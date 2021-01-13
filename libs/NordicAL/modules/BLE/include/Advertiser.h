/**
 * @file Advertiser.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * @brief Port-specific class for Nordic SDK BLE. Implements advertiser class,
 * which is used within aconno BLE library to properly handle advertising 
 * multiple advertisements simultaneously, with support for advertisement 
 * burst mode.
 * By default, Nordic SDK only supports one advertisement set, so this class
 * multiplexes it.
 * 
 * Advertiser is implemented using eager-loading singleton design pattern,
 * as there should only ever be one per module.
 *
 * @version 1.0
 * @date 2020-06-08
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __ADVERTISER_H__
#define __ADVERTISER_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace IO::BLE
{
class Advertiser;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Advertisement.h"

#include <AL_Event.h>
#include <AL_EventGroup.h>
#include <AL_Queue.h>
#include <AL_Task.h>
#include <aconnoConfig.h>
#include <ble.h>
#include <ble_advdata.h>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class Advertiser {
    static constexpr size_t kStackSize =
        256; /**< Size of the advertiser task stack in sizeof(StackType_t) bytes */

    // Befriend Advertisement to allow it to put itself into Adv queue
    friend IO::BLE::Advertisement;
    friend RTOS::Task<kStackSize, Advertiser>;

    // Delete default constructors
    Advertiser(const Advertiser& other) = delete;
    Advertiser& operator=(const Advertiser& other) = delete;

private:
    Advertiser();

    /**
     * @brief struct used to queue advertisement
     */
    struct Data {
        TxPower  txPower;
        uint8_t  burstCount;
        uint8_t* data;
        size_t   len;
    };

    /**
     * Size of the queue containing advertisement to be broadcasted.
     * Value chosen arbitrarily, based on experience.
     */
    static constexpr uint8_t kAdvQueueSize = 25;

    /** Total number of events defined in Advertiser class. */
    static constexpr uint8_t kNumOfEvents = 2;

public:
    static Advertiser& getInstance();
    static void        onConnect();
    static void        reset();

private:
    static Advertiser                  instance;
    RTOS::EventGroup                   radioEvents;
    RTOS::Event                        advBurstCompletedEvent;
    RTOS::Event                        deviceConnectedEvent;
    RTOS::EventList<kNumOfEvents>      radioEventsList;
    RTOS::Queue<Data, kAdvQueueSize>    advertisementQueue;
    RTOS::Task<kStackSize, Advertiser> rtosTask;

    Data advToBroadcast; /**< Holds advertisement instance received from adv. queue to be broadcast next */

    void onStart();
    void onRun();

    static constexpr uint32_t msToAdvIntervalUnits(RTOS::milliseconds ms);
    static constexpr uint16_t msToAdvDurationUnits(RTOS::milliseconds ms);
    static void eventHandler(ble_evt_t const* p_ble_evt, void* p_context);

    /*--- Constants ---*/

    /** Error code. 
	 * Shall later be replaced by error reason for restart.
	 */
    static constexpr uint8_t ADV_ERROR_CODE = 0x00;

    /**
	 * Determines wheather and how device name shall be included in advertising data.
	 * Not obligatory. If enabled, sends device name as part of the advertisement, 
	 * reserving (2 + name size) bytes out of 31 adv. bytes available.
	 */
    static constexpr ble_advdata_name_type_t ADV_DEVICE_NAME =
        BLE_ADVDATA_NO_NAME;

    /**
	 * Determines wheather Appearance shall be included in advertising data.
	 * Appearance consists of 2 bytes that give other devices decription of 
	 * the device according to Bluetooth standard categories (i.e. mobile phone, 
	 * heart rate device, key ring, etc.).
	 * It is not obligatory. 
	 * If included, it reserves 4 bytes of 31 adv. bytes available.
	 */
    static constexpr bool ADV_APPEARANCE = false;

    /**
	 * Defines advertisement flags included in advertising data.
	 * Flags are obligatory by standard for connectable advertisements. 
	 * Check BLE_GAP_ADV_FLAGS GAP.
	 * When enabled, flags reserve 3 bytes out of 31 adv. bytes available.
	 */
    static constexpr uint8_t ADV_FLAGS =
        BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    /**
	 * First advertisement after initialization contains ADV_INIT_PRODUCT_ID value
	 * in place of PRODUCT_ID to signal that device is started/restarted.
	 */
    static constexpr uint8_t ADV_INIT_PRODUCT_ID = 0xFF;

    /**
     * Max. time to give to SD task to perform advertisement burst.
     * After this time passes, SD is considered to be in error state and unresponsive.
     * Guesstimate based on max. allowed number of advertisements in the burst, advertisement 
     * sending times as per BT standard and context switching durations, with an additional safety
     * multiplier added in.
     */
    static constexpr RTOS::milliseconds kCriticalTimeout = 5000;
};

}  // namespace IO::BLE

#endif  //__ADVERTISER_H__
