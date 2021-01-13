/**
 * @file	AL_DFU.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief 	Implements system update via BLE using Nordic DFU module
 * 
 * @details	This class implements OTA (Over-The-Air) update feature using Nordic
 * 			DFU module. DFU (Direct-Firmware-Update) module utilizes BLE
 * 			service and characteristic to perform OTA update of application
 * 			alone, or application, softdevice and bootloader all together.
 * 
 * 			DFU updates are initiated by writing a specific value into DFU
 * 			characteristic, which appears automatically when ENABLE_DFU_UPDATES
 * 			flag is set to YES. In response to it, firmware writes a value in
 * 			certain permanent register which bootloader reads at system startup.
 * 			If value is present there, bootloader boots into DFU mode instead of
 * 			the application. This is where actual DFU update is applied.
 * 
 * 			However, before rebooting after writing a value to permanent
 * 			register, device has to shut down properly. This class implements
 * 			handlers which make sure device safely stores any critical
 * 			information and shuts down properly.
 * 
 * @version 0.1.0
 * @date 	2020-09-17
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_DFU_H__
#define __AL_DFU_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace SYS
{
class DFU;
}

//--------------------------------- INCLUDES ----------------------------------
extern "C" {
#include "nrf_pwr_mgmt.h"  // this header does not have C++ extern protection
}

#include "ble_dfu.h"
#include "nrf_sdh.h"

namespace SYS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class DFU {
    // Delete default constructors
    DFU()                 = delete;
    DFU(const DFU& other) = delete;
    DFU& operator=(const DFU& other) = delete;

public:
    /* --- Public API --- */
    static void init();

private:
    /* --- Private API --- */
    static void eventHandler(ble_dfu_buttonless_evt_type_t event);
    static bool shutdownHandler(nrf_pwr_mgmt_evt_t event);
};

}  // namespace SYS
#endif  //__AL_DFU_H__