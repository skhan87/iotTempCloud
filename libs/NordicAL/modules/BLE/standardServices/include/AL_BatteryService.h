/**
 * @file 
 * @author 
 * @brief 
 * @version
 * @date
 *
 * @copyright
 *
 */

#ifndef __AL_BATTERYSERVICE_H__
#define __AL_BATTERYSERVICE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace BLE
{
class BatteryService;
}

//--------------------------------- INCLUDES ----------------------------------

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "sdk_config.h"
#include "ble_bas.h"
#include "sdk_errors.h"

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief 
 */
class BatteryService
{
    
public:
	// Delete default constructors
	BatteryService(const BatteryService &other) = delete;
	BatteryService &operator=(const BatteryService &other) = delete;

	// Class constructor
	BatteryService();

	void init();

	ret_code_t updateBatteryLevel(uint8_t batteryLevel);

private:
ble_bas_t serviceStruct; /**< Battery service instance. */

};  
} // namespace BLE


#endif  //__AL_BATTERYSERVICE_H__