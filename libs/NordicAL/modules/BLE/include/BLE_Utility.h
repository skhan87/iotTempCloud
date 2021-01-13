/**
 * @file BLE_Utility.h
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
#ifndef __BLEUTILITY_H__
#define __BLEUTILITY_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace IO::BLE
{
class Utility;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_DFU.h"

#include <AL_Eddystone.h>
#include <AL_Service.h>
#include <Advertiser.h>
#include <CharacteristicBase.h>
#include <LifetimeList.h>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class Utility {
    // Delete default constructors
    Utility()                     = delete;
    Utility(const Utility& other) = delete;
    Utility& operator=(const Utility& other) = delete;

    // Befriend internal library classes
    friend IO::BLE::Advertiser;
    friend IO::BLE::Service;
    friend IO::BLE::CharacteristicBase;
    friend SYS::DFU;

    template<class T>
    friend class Characteristic;

public:
    static void init();
    static bool isConnected();

    /**
	 * A tag identifying the SoftDevice BLE configuration.
	 */
    static constexpr uint8_t APP_BLE_CONN_CFG_TAG = 1;

private:
    static Eddystone<3> connectable; /**< Declaration of default advertisement. */
    static uint16_t
        uuidCount; /**< Simple counter used as generic service/characteristic UUID */
    static uint16_t
        devConnHandle; /**< Handle of the current device connection. */

    static void initBLEStack();
    static void initGAP();
    static void initGATT();

    static void softdeviceBLEEventHandler(ble_evt_t const* p_ble_evt,
                                          void*            p_context);

    /*--- Private constants ---*/
    /**
	 * Application's BLE observer priority.
	 * You shouldn't need to modify this value.
	 */
    static constexpr uint8_t APP_BLE_OBSERVER_PRIO = 0;

    /**
	 * Minimum acceptable connection interval (0.4 seconds).
	 */
    static constexpr uint16_t MIN_CONN_INTERVAL =
        MSEC_TO_UNITS(400, UNIT_1_25_MS);

    /**
	 * Maximum acceptable connection interval (0.65 second).
	 */
    static constexpr uint16_t MAX_CONN_INTERVAL =
        MSEC_TO_UNITS(650, UNIT_1_25_MS);

    /**
	 * Slave latency.
	 */
    static constexpr uint16_t SLAVE_LATENCY = 0;

    /**
	 * Connection supervisory time-out (4 seconds).
	 */
    static constexpr uint16_t CONN_SUP_TIMEOUT =
        MSEC_TO_UNITS(4000, UNIT_10_MS);
};  // class Utility

}  // namespace IO::BLE

#endif  //__BLEUTILITY_H__
