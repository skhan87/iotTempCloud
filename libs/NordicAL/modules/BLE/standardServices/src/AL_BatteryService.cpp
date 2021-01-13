/**
 * @file 
 * @author 
 * @brief 
 * @version 
 * @date 
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_BatteryService.h"
#include "app_error.h"


//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
IO::BLE::BatteryService::BatteryService()
{	
}

void IO::BLE::BatteryService::init()
{
	uint32_t err_code;

	NRF_SDH_BLE_OBSERVER(m_bas_obs,            
                    BLE_BAS_BLE_OBSERVER_PRIO,
                    ble_bas_on_ble_evt,      
                    &this->serviceStruct);

	ble_bas_init_t     bas_init;   

	// Initialize Battery Service.
	memset(&bas_init, 0, sizeof(bas_init));

	// Here the sec level for the Battery Service can be changed/increased.
	bas_init.bl_rd_sec        = SEC_OPEN;
	bas_init.bl_cccd_wr_sec   = SEC_OPEN;
	bas_init.bl_report_rd_sec = SEC_OPEN;

	bas_init.evt_handler          = NULL;
	bas_init.support_notification = true;
	bas_init.p_report_ref         = NULL;
	bas_init.initial_batt_level   = 100;

	err_code = ble_bas_init(&this->serviceStruct, &bas_init);
	APP_ERROR_CHECK(err_code);
}
                                           
//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
ret_code_t IO::BLE::BatteryService::updateBatteryLevel(uint8_t batteryLevel)
{
    ret_code_t err_code;

    err_code = ble_bas_battery_level_update(&this->serviceStruct, batteryLevel, BLE_CONN_HANDLE_ALL);

	return err_code;
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
