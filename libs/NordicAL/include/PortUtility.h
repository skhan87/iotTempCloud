/**
 * @file PortUtility.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Nordic SDK Utility class.
 * @version 1.0
 * @date 2020-06-08
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __PORTUTILITY_H__
#define __PORTUTILITY_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::Flash
{
class Utility;
class File;
template<class T>
class Record;
}  // namespace IO::Flash

namespace Port
{
class Utility;
}

//--------------------------------- INCLUDES ----------------------------------

#include "LoggerTask.h"

#include <AL_AnalogIn.h>
#include <AL_DFU.h>
#include <AL_InterruptIn.h>

#include "Scanner.h"

#include <AL_Service.h>
#include <Advertiser.h>
#include <BLE_Utility.h>
#include <CharacteristicBase.h>
#include <AL_I2CBus.h>
#include <Error.h>
#include <fds.h>
#include <sdk_config.h>

#if NRFX_TWIM_ENABLED
#include <AL_I2CBus.h>
#endif

#if NRFX_PWM_ENABLED
#include <AL_PWM.h>
#endif

namespace Port
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Nordic SDK Utility class.
 * Based on utility pattern.
 */
class Utility {
    // delete default constructors
    Utility()                     = delete;
    Utility(const Utility& other) = delete;
    Utility& operator=(const Utility& other) = delete;

    // befriend internal library classes
    friend IO::BLE::Utility;
    friend IO::BLE::Advertiser;
    friend IO::BLE::Service;
    friend IO::BLE::CharacteristicBase;
    friend IO::BLE::Scanner;
    friend IO::InterruptIn;
    friend IO::AnalogIn;
    friend SYS::DFU;
    friend IO::I2C::Bus;

#if NRF_LOG_ENABLED
    friend Log::Task;
#endif

#if NRFX_PWM_ENABLED
    friend IO::PWM;
#endif

#if NRFX_TWIM_ENABLED
    friend IO::I2C::Bus;
#endif  // NRFX_TWIM_ENABLED

    friend class IO::Flash::Utility;
    template<class T>
    friend class IO::Flash::Record;
    friend class IO::Flash::File;

private:
    /**
     * @brief Transforms a nordic error code into an aconno one.
     * 
     * @param nordicErrorCode 
     * @return constexpr Error::Code 
     */
    static constexpr Error::Code getError(uint32_t nordicErrorCode)
    {
        if (nordicErrorCode == NRF_SUCCESS) {
            return Error::None;
        }

        switch (nordicErrorCode) {
            case NRF_ERROR_INVALID_ADDR:  // Invalid or NULL pointer supplied.
                return Error::Alignment;
            case NRF_ERROR_INVALID_PARAM:  // Invalid parameter(s) supplied.
                return Error::InvalidUse;
            case NRF_ERROR_SOFTDEVICE_NOT_ENABLED:
                return Error::NotInitialized;
            case NRF_ERROR_DATA_SIZE:  // Invalid data size(s) supplied.
                return Error::InvalidUse;
            case NRF_ERROR_INVALID_STATE:  // Invalid state to perform operation.
            case NRF_ERROR_INTERNAL:
                return Error::Internal;
            case NRF_ERROR_INVALID_FLAGS:  // Invalid combination of advertising flags supplied.
                return Error::InvalidParameter;
            case NRF_ERROR_INVALID_LENGTH:  // Invalid data length(s) supplied.
                return Error::InvalidParameter;
            case NRF_ERROR_INVALID_DATA:  // Invalid data type(s) supplied.
                return Error::InvalidParameter;
            case NRF_ERROR_NO_MEM:  // Not enough memory to configure a new advertising handle.
                return Error::Memory;
            case NRF_ERROR_NULL:  // Null Pointer
                return Error::Memory;
            case NRF_ERROR_NOT_SUPPORTED:  // Unsupported data length or advertising parameter configuration or file not opened in fds.
                return Error::InvalidUse;
            case BLE_ERROR_GAP_INVALID_BLE_ADDR:  // ble_gap_adv_params_t::p_peer_addr is invalid.
                return Error::InvalidParameter;
            case BLE_ERROR_GAP_DISCOVERABLE_WITH_WHITELIST:  // Discoverable mode and whitelist incompatible.
                return Error::InvalidUse;
            case BLE_ERROR_GAP_UUID_LIST_MISMATCH:  // Invalid UUID list supplied.
                return Error::InvalidUse;
            case BLE_ERROR_INVALID_ADV_HANDLE:  // The provided advertising handle was not found.
                return Error::InvalidParameter;
            case BLE_ERROR_INVALID_CONN_HANDLE:  // Invalid connection handle supplied.
                return Error::InvalidParameter;
            case NRF_ERROR_CONN_COUNT:  // The limit of available connections has been reached; connectable advertiser cannot be started.
                return Error::OutOfResources;
            case NRF_ERROR_NOT_FOUND:  // conn_cfg_tag not found.
                return Error::NotFound;
            case NRF_ERROR_RESOURCES:  // Either:
                //  - adv_handle is configured with connectable advertising, but the event_length parameter
                //   associated with conn_cfg_tag is too small to be able to establish a connection on
                //   the selected advertising phys. Use @ref sd_ble_cfg_set to increase the event length.
                //  - Not enough BLE role slots available.
                //   Stop one or more currently active roles (Central, Peripheral, Broadcaster or Observer) and try again.
                //  - p_adv_params is configured with connectable advertising, but the event_length parameter
                //   associated with conn_cfg_tag is too small to be able to establish a connection on
                //   the selected advertising phys. Use @ref sd_ble_cfg_set to increase the event length.
                return Error::OutOfResources;
            case NRF_ERROR_SVC_HANDLER_MISSING:  // SVC handler is missing
                return Error::InvalidUse;
            case NRF_ERROR_FORBIDDEN:
                return Error::InvalidUse;
            case NRFX_ERROR_BUSY:
                return Error::Busy;
            case NRFX_ERROR_DRV_TWI_ERR_ANACK:  // I2C address NACK error
            case NRFX_ERROR_DRV_TWI_ERR_DNACK:  // I2C data byte NACK error
                return Error::Acknowledgement;
            default:
                // nordic error code not yet translated, implement it!
                // this shall never happen in productive.
                return Error::Unknown;
        }
    }
};
}  // namespace Port
#endif  //__PORTUTILITY_H__
