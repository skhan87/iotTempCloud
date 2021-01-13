/**
 * @file BLE_Utility.cpp
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief Utility class for aconno BLE module.
 * 
 * @details This file is part of the implementation of aconno BLE Utility class. 
 * 			Based on utility pattern, BLE Utility class contains all facilities 
 * 			of aconno BLE module which have to be shared between several 
 * 			BLE module objects.
 * 
 * @version 1.0
 * @date 2020-7-7
 * 
 * @copyright aconno GmbH (c) 2020
 */

//--------------------------------- INCLUDES ----------------------------------
#include "nrf_ble_gatt.h"
#include "nrf_sdh_ble.h"

#include <AL_Log.h>
#include <BLE_Utility.h>
#include <Error.h>
#include <PortUtility.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------
NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */

uint16_t IO::BLE::Utility::
    devConnHandle; /**< Device connection handle. Used to determine status of device connection. */

IO::BLE::Eddystone<3>
    IO::BLE::Utility::connectable(Config::BLE_CONNECTABLE_ADV_INTERVAL,
                                  IO::BLE::TxPower::p0dB,
                                  IO::BLE::CompanySigId::NordicSemiconductor,
                                  {0xD, 0xE, 0xF},
                                  1,
                                  false);

uint16_t IO::BLE::Utility::uuidCount = 0;

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Initializes all aconno BLE module libraries.
 */
void IO::BLE::Utility::init()
{
    initBLEStack();
    initGAP();
    initGATT();

    // Check if user services exist
    if (Service::getList().size() != 0) {
        // Go through list of them and initialize them with softdevice
        for (auto& service : Service::getList()) {
            service.init();

            // Find and initialize all characteristics belonging to this service
            for (auto& characteristic : CharacteristicBase::getList()) {
                if (&characteristic.getService() == &service) {
                    characteristic.init();
                }
            }
        }

        // Custom services exist! Start default connectable advertisement.
        IO::BLE::Utility::connectable.start();
    }

    LOG_I("aconno BLE module initialized.");
}

/**
 * @brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void IO::BLE::Utility::initBLEStack()
{
    ret_code_t err_code;

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    if (err_code != NRF_SUCCESS) {
        CHECK_ERROR(Error::Memory);
    }

    Utility::devConnHandle = BLE_CONN_HANDLE_INVALID;

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    CHECK_ERROR(Port::Utility::getError(err_code));

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer,
                         APP_BLE_OBSERVER_PRIO,
                         softdeviceBLEEventHandler,
                         nullptr);
}

/**
 * @brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 * 
 * 			The Generic Access Profile (GAP) provides the framework that defines how BLE devices
 * 			interact with each other. The implementation of this framework is mandatory per 
 * 			the official specification, and it is what allows two or more BLE devices to interoperate, 
 * 			communicate, and be able to exchange data with each other.
 * 
 * @return  None.
 */
void IO::BLE::Utility::initGAP()
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    // Set Device Name characteristic of GAP service
    err_code =
        sd_ble_gap_device_name_set(&sec_mode,
                                   (const uint8_t*)Config::DEVICE_NAME,
                                   strlen((const char*)Config::DEVICE_NAME));
    CHECK_ERROR(Port::Utility::getError(err_code));

    // Set Appearance characteristic of GAP service
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    CHECK_ERROR(Port::Utility::getError(err_code));

    // Configure Peripheral Preferred Connection Parameters (PPCP) characteristic of GAP service
    // Once connected, central device can (but doesn't have to) read and respect these parameters)
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    // Set GAP Peripheral Preferred Connection Parameters
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    CHECK_ERROR(Port::Utility::getError(err_code));
}

/**
 * @brief Function for initializing the GATT module.
 */
void IO::BLE::Utility::initGATT()
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    CHECK_ERROR(Port::Utility::getError(err_code));
}

/**
 * @brief Function for handling BLE Stack events related to our service and characteristic.
 * 
 * @details Handles all events from the BLE stack of interest to Our Service.
 * 
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void IO::BLE::Utility::softdeviceBLEEventHandler(ble_evt_t const* p_ble_evt,
                                                 void*            p_context)
{
    ret_code_t err_code;

    ble_gap_evt_t const* p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            LOG_I("BLE event: Connected.");
            Utility::connectable.stop();
            Advertiser::getInstance().onConnect();
            IO::BLE::Utility::devConnHandle =
                p_ble_evt->evt.gap_evt.conn_handle;
            break;
        }

        case BLE_GAP_EVT_DISCONNECTED: {
            LOG_I("BLE event: Disconnected.");
            auto reason = p_ble_evt->evt.gap_evt.params.disconnected.reason;
            IO::BLE::Service::onDisconnect(reason);
            Utility::connectable.start();
            IO::BLE::Utility::devConnHandle = BLE_CONN_HANDLE_INVALID;
            break;
        }

        case BLE_GAP_EVT_TIMEOUT: {
            // We have not specified a timeout for scanning, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
                LOG_D("Connection request timed out.");
            }
            break;
        }

        case BLE_GAP_EVT_CONN_PARAM_UPDATE: {
            LOG_I("Connection interval updated: 0x%x, 0x%x.",
                  p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params
                      .min_conn_interval,
                  p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params
                      .max_conn_interval);
            break;
        }

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(
                p_ble_evt->evt.gap_evt.conn_handle,
                &p_ble_evt->evt.gap_evt.params.conn_param_update_request
                     .conn_params);
            CHECK_ERROR(Port::Utility::getError(err_code));
            break;
        }

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            LOG_D("BLE event: PHY update request.");
            ble_gap_phys_t const phys = {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle,
                                             &phys);
            CHECK_ERROR(Port::Utility::getError(err_code));
            break;
        }

        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(devConnHandle, NULL, 0, 0);
            CHECK_ERROR(Port::Utility::getError(err_code));
            break;
        }

        case BLE_GATTC_EVT_TIMEOUT: {
            // Disconnect on GATT Client timeout event.
            LOG_D("BLE event: GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gattc_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            CHECK_ERROR(Port::Utility::getError(err_code));
            break;
        }

        case BLE_GATTS_EVT_TIMEOUT: {
            // Disconnect on GATT Server timeout event.
            LOG_D("BLE event: GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gatts_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            CHECK_ERROR(Port::Utility::getError(err_code));
            break;
        }

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
            // Currently caused by DFU and handled in DFU module.
            break;
        }

        case BLE_GATTS_EVT_HVC: {
            // Indication was successful
            LOG_D("BLE event: Indication confirmation received.");
            break;
        }

        case BLE_GATTS_EVT_HVN_TX_COMPLETE: {
            LOG_D("BLE event: Notification TX completed.");
            break;
        }

        case BLE_GATTS_EVT_WRITE: {
            CharacteristicBase::forwardEvent(
                p_ble_evt->evt.gatts_evt.conn_handle,
                p_ble_evt->evt.gatts_evt.params.write);
            break;
        }

        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP: {
            LOG_D(
                "MTU size changed to %u",
                p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu);
            break;
        }

        // List cases handled elsewhere
        case BLE_GAP_EVT_ADV_REPORT:
        case BLE_GAP_EVT_ADV_SET_TERMINATED: {
            break;
        }

        default:
            LOG_D("BLE event: Unknown event occured: %u",
                  p_ble_evt->header.evt_id);
            break;
    }
}

/**
 * @brief Helper function used to find out if devic is currently in connection.
 * 
 * @return True if device is in connected state.
 * 		   False if device is not in connected state.
 */
bool IO::BLE::Utility::isConnected()
{
    return (devConnHandle != BLE_CONN_HANDLE_INVALID);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
