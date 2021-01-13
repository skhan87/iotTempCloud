/**
 * @file	AL_DFU.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief 	Impements system update via BLE-
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

//--------------------------------- INCLUDES ----------------------------------
#include "AL_DFU.h"
#include "AL_Log.h"
#include "BLE_Utility.h"
#include "Error.h"
#include "PortUtility.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
/**
 * @brief Initializes DFU functionality.
 *
 * @return  None.
 */
void SYS::DFU::init()
{
    ret_code_t                errCode;
    ble_dfu_buttonless_init_t dfus_init = {0};

    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    errCode = ble_dfu_buttonless_async_svci_init();
    CHECK_ERROR(Port::Utility::getError(errCode));

    // Register application shutdown handler with priority 0.
    NRF_PWR_MGMT_HANDLER_REGISTER(shutdownHandler, 0);

    // Initialize DFU service
    dfus_init.evt_handler = eventHandler;

    errCode = ble_dfu_buttonless_init(&dfus_init);
    CHECK_ERROR(Port::Utility::getError(errCode));
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
/**
 * @brief Function for handling event reported by DFU service.
 *
 * @param	event	Event from the Buttonless Secure DFU service.
 * 
 * @return None.
 */
void SYS::DFU::eventHandler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event) {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE: {
            LOG_I("Device is preparing to enter bootloader mode.");
            // TODO: prevent device from advertising on disconnect.
            // TODO: disconnect ALL devices that currently are connected.
            ret_code_t err_code = sd_ble_gap_disconnect(
                IO::BLE::Utility::devConnHandle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);

            if (err_code != NRF_SUCCESS) {
                LOG_E("Failed to disconnect connection."
                      "Connection handle: %d Error: %d",
                      IO::BLE::Utility::devConnHandle,
                      err_code);
            } else {
                LOG_D("Disconnected connection handle %d",
                      IO::BLE::Utility::devConnHandle);
            }
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            /* At this point write app-specific unwritten data to FLASH, 
				control finalization of this by delaying reset by reporting 
				false in shutdownHandler. */
            LOG_I("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            LOG_E("Request to enter bootloader mode failed asynchroneously.");
            // Reseting device to recover
            // TODO: create dfu-specific error flag
            CHECK_ERROR(Error::Unknown);
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            LOG_E("Request to send a response to client failed.");
            // Reseting device to recover
            CHECK_ERROR(Error::Unknown);
            break;

        default:
            LOG_I("Unknown event from DFU module.");
            break;
    }
}

/**
 * @brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at 
 * 			a 1 second interval until the function returns true. 
 * 			When the function returns true, it means that the app is ready to 
 * 			reset to DFU mode.
 *
 * @param	event	Power manager event.
 *
 * @return  True if shutdown is allowed by this power manager handler, 
 * 			otherwise false.
 */
bool SYS::DFU::shutdownHandler(nrf_pwr_mgmt_evt_t event)
{
    switch (event) {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU: {
            LOG_I("Power management wants to reset to DFU mode.");
            /* Get ready to reset into DFU mode.
            	If you aren't finished with any ongoing tasks, return "false" to
            	signal to the system that reset is impossible at this stage. */

            // Device ready to enter DFU
            auto errCode = nrf_sdh_disable_request();
            CHECK_ERROR(Port::Utility::getError(errCode));

            bool softdeviceNotReady = nrf_sdh_is_enabled();
            if (softdeviceNotReady) {
                // Wait another second and retry.
                return false;
            } else {
                NRF_LOG_INFO("SD disabled");
            }

            break;
        }

        case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
            LOG_D("DFU power management prepare sysoff event.");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            LOG_D("DFU power management prepare wakeup event.");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_RESET:
            LOG_D("DFU power management prepare reset event.");
            break;

        default:
            LOG_I("Unknown DFU power management event.");
            return false;
    }

    LOG_I("Power management allowed to reset to DFU mode.");
    return true;
}
