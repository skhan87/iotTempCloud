/**
 * @file AL_I2CDevice.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief 
 * @version 0.1
 * @date 2020-09-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <sdk_config.h>

#if NRFX_TWIM_ENABLED

#ifndef __AL_I2CDEVICE_H__
#define __AL_I2CDEVICE_H__

#include <nrf_twim.h>
#include <AL_I2CBus.h>
#include <PortUtility.h>
#include "Endians.h"

namespace IO::I2C
{
/**
 * @brief I2C (TWI) device abstaction.
 * 
 * @details One instance of this class represents one I2C slave on the assigned 
 *          bus. You can easily read and write registers with this class.
 * 
 * @warning This driver uses 7-bit I2C addresses.
 *          In I2C, least significant bit is toggled as part of the communication
 *          protocol, to denote whether current packed is write or read packet.
 *          Therefore, actual I2C address of the device is xxxx xxx(R/W).
 * 
 *          Some manufacturers specify 7-bit address in device datasheets,
 *          others specify address with 0 as the last bit, xxxx xxx0.
 * 
 *          Care has to be taken while determining which convention the 
 *          hardware manufacturer used.
 * 
 * @tparam registerAddressType Type of the register address. uint8_t or uint16_t.
 * @tparam registerAddressEndianess Endianess of the register addresses. Only applicable for uint16_t.
 */
template<class registerAddressType                   = uint8_t,
         Endians::ByteOrder registerAddressEndianess = Endians::ByteOrder::big>
class Device {
    // Delete default constructor
    Device()                    = delete;
    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

public:
    // constructor
    Device(Bus&      bus,
           uint8_t   deviceAddress,
           Frequency frequency = Frequency::_100K);

    Error::Code setRegisters(uint8_t*            values,
                             size_t              length,
                             registerAddressType registerAddress,
                             RTOS::milliseconds  timeout = RTOS::Infinity);

    Error::Code getRegisters(uint8_t*            values,
                             size_t              length,
                             registerAddressType registerAddress,
                             RTOS::milliseconds  timeout = RTOS::Infinity);

protected:
    uint8_t getDeviceAddress();
    void    setDeviceAddress(uint8_t address);

private:
    Bus&      bus; /**< Bus instance used by the device. */
    Frequency frequency; /**< Device specific frequency. */
    uint8_t deviceAddress; /**< Address of the device connected with the I2C */
};
}  // namespace IO::I2C

#include "../src/AL_I2CDevice.cpp"
#endif  // __AL_I2CDEVICE_H__
#endif