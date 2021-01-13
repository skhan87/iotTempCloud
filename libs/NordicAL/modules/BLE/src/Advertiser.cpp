/**
 * @file Advertiser.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * @brief Implementation of aconno BLE Advertiser class.
 * @details Port-specific class for Nordic SDK BLE. Implements advertiser class,
 * 			which is used within aconno BLE library to properly handle 
 * 			advertising multiple advertisements simultaneously, with support
 * 			for advertisement burst mode.
 * 
 * 			By default, Nordic SDK only supports one advertisement set, so this
 * 			class multiplexes it.
 * 
 * 			Advertiser is implemented using eager-loading singleton design
 * 			pattern, as there should only ever be one per module.
 *
 * @version 1.0
 * @date 2020-06-08
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Advertiser.h"

#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include <AL_Log.h>
#include <BLE_Utility.h>
#include <Error.h>
#include <PortUtility.h>
#include <aconnoConfig.h>
#include <ble_gap.h>
#include <memory>

//--------------------------- STRUCTS AND ENUMS -------------------------------
IO::BLE::Advertiser IO::BLE::Advertiser::instance {};

// Handle issued by Nordic SDK to identify the advertisement
static uint8_t advHandle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

static ble_gap_adv_data_t   advData;
static ble_gap_adv_params_t advParameters;

// List variables that are to be defined by user per project
extern const char* const Config::DEVICE_NAME;

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Advertiser task constructor.
 * 
 * @details As there should always be only one advertiser per module, advertiser
 * 			is implemented according to singleton design pattern. Note that in 
 * 			singleton pattern no public constructors are available.
 */
IO::BLE::Advertiser::Advertiser()
        : radioEvents {}, advBurstCompletedEvent {radioEvents},
          deviceConnectedEvent {radioEvents},
          radioEventsList {&advBurstCompletedEvent, &deviceConnectedEvent},
          advertisementQueue {"advQueue"}, rtosTask {*this, "Advertiser", 3}
{
    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_adv_observer,
                         Utility::APP_BLE_OBSERVER_PRIO,
                         eventHandler,
                         nullptr);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Implementation of task Context
 * 
 */
void IO::BLE::Advertiser::onStart()
{
    // do nothing
}

/**
 * @brief Implements functionality of the advertiser task.
 *
 * @details This task is unblocked when there is new advertisement to be 
 * 			broadcasted. It updates advertisement data and parameters to match
 * 			the advertisement which is to be broadcasted.
 * 
 *          Broadcasting advertisements is disrupted when a new device connection is formed.
 *          This is due to Nordic stack invoking sd_adv_stop() at connect event. Advertiser has
 *          to recover from this, and will drop the subsequent advertisement burst as a result.
 *          As advertisements by their very nature are not time critical nor guaranteed to arrive
 *          at every interval, this is deemed acceptable.
 * 
 * @return  None.
 */
void IO::BLE::Advertiser::onRun()
{
    // Block waiting on advertisement to become available in adv. queue.
    auto errCode = advertisementQueue.receive(advToBroadcast, RTOS::Infinity);
    if (errCode != Error::None) {
        LOG_E("Receiving from advertisement queue failed: %u", errCode);
        return;
    }

    // Delete dynamic memory on return
    std::unique_ptr<uint8_t> data {std::move(advToBroadcast.data)};

    // Reset advetisement parameters struct
    memset(&advParameters, 0, sizeof(advParameters));
    advParameters.p_peer_addr = nullptr;
    advParameters.interval = msToAdvIntervalUnits(Advertisement::MIN_INTERVAL);
    advParameters.duration = 0;  // unlimited
    advParameters.max_adv_evts =
        advToBroadcast.burstCount;  // number of advertisements before stopping.
    advParameters.filter_policy = BLE_GAP_ADV_FP_ANY;
    advParameters.primary_phy =
        BLE_GAP_PHY_AUTO;  // will forward to BLE_GAP_PHY_1MBPS and ignored when not extended adv

    // Once in connection, device cannot broadcast connectable advertisements.
    // Advertisements that still want to advertise are made nonconnectable.
    if (Utility::isConnected()) {
        advParameters.properties.type =
            BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    } else {
        advParameters.properties.type =
            BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    }

    // Define data to advertise
    advData.adv_data.p_data      = data.get();
    advData.adv_data.len         = (uint16_t)advToBroadcast.len;
    advData.scan_rsp_data.p_data = nullptr;
    advData.scan_rsp_data.len    = 0;

    // Initialize the adv package, do not yet supply any data
    errCode = Port::Utility::getError(
        sd_ble_gap_adv_set_configure(&advHandle, &advData, &advParameters));
    if (errCode != Error::None) {
        LOG_D("Failed to configure gap advertisement: %u", errCode);
        return;
    }

    // Settable only after m_advertising handle has be initialized
    errCode = Port::Utility::getError(
        sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,
                                advHandle,
                                (int8_t)advToBroadcast.txPower));
    if (errCode != Error::None) {
        LOG_D("Failed to the advertisement set tx power value: %u", errCode);
        return;
    }

    radioEvents.resetEvents(radioEventsList);

    errCode = Port::Utility::getError(
        sd_ble_gap_adv_start(advHandle, Utility::APP_BLE_CONN_CFG_TAG));
    if (errCode != Error::None) {
        LOG_D("Failed to start the advertising: %u", errCode);
        LOG_D(" --- expected to occur when a device connects to beacon.");
        return;
    }

    // Wait until the advertisement burst is finished broadcasting or device connects.
    errCode = radioEvents.await(radioEventsList,
                                kCriticalTimeout,
                                RTOS::EventGroup::WaitMode::Or);

    if (errCode != Error::None) {
        if (errCode == Error::Timeout) {
            LOG_E("Advertisement timed out. Reseting Advertiser.");
        } else {
            LOG_E("Unknown Advertiser error, resetting.");
        }
        reset();
    }
}

/**
 * @brief Contains functionality to be executed when connection event occurs.
 * 
 * @details Upon ble connection event softdevice (or some other part of the stack) calls 
 *          the sd_adv_stop() function automatically. Due to this, adv. finished event is
 *          never triggered, and advertiser falsely times out. 
 *  
 *          This is handled now by raising adv. finished event ourselves using this function when 
 *          connection event occurs. As sd is stopped then, the event will not happen on its own so
 *          we do this to unblock advertiser task. Advertiser shall move to next advertisement, 
 *          run the sd_adv_start() for it and continue advertising in non-connectable mode normally.
 */
void IO::BLE::Advertiser::onConnect()
{
    getInstance().deviceConnectedEvent.trigger();
}

/**
 * @brief Resets advertiser module.
 * 
 * @details Stops the softdevice advertising. This will disrupt any stuck broadcast. Events are
 *          reset to reflect that. Softdevice will automatically be started again at next
 *          advertiser task iteration.
 */
void IO::BLE::Advertiser::reset()
{
    sd_ble_gap_adv_stop(advHandle);
    getInstance().radioEvents.resetEvents(getInstance().radioEventsList);
}

/**
 * @brief Obtain advertiser task instance if required.
 * 
 * @return Single Advertiser instance.
 */
IO::BLE::Advertiser& IO::BLE::Advertiser::getInstance()
{
    return instance;
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Transforms ms to advertisement interval units used by Nordic SDK.
 * 
 * @details One advertismenet unit is 0,625ms.
 * 
 * @param	millis		Time interval in milliseconds.
 * @return	uint32_t	Time interval in Nordic SDK units.
 */
uint32_t constexpr IO::BLE::Advertiser::msToAdvIntervalUnits(
    RTOS::milliseconds millis)
{
    return (uint32_t)(MSEC_TO_UNITS(millis, UNIT_0_625_MS));
}

/**
 * @brief Transforms ms to advertisement duration units used by Nordic SDK.
 * 
 * @details One duration unit is 10ms.
 * 
 * @param	millis		Time duration in milliseconds. 
 * @return	uint16_t	Time duration in Nordic SDK units.
 */
uint16_t constexpr IO::BLE::Advertiser::msToAdvDurationUnits(
    RTOS::milliseconds millis)
{
    return (uint16_t)(MSEC_TO_UNITS(millis, UNIT_10_MS));
}

/**
 * @brief handler for softdevice BLE events
 * 
 * @param p_ble_evt 
 * @param p_context 
 */
void IO::BLE::Advertiser::eventHandler(ble_evt_t const* p_ble_evt,
                                       void*            p_context)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_ADV_SET_TERMINATED:
            // finished advertising the last advertisement burst,
            // unblock the advertiser task
            instance.advBurstCompletedEvent.trigger();
            break;
    }
}
