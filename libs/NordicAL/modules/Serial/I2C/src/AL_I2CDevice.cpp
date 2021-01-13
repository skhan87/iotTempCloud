/**
 * @file AL_I2CDevice.cpp
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

//--------------------------------- INCLUDES ----------------------------------

#include <AL_I2CDevice.h>
#include <type_traits>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Construct a I2C slave device.
 * 
 * @param busInstance Bus instance to which this device is connected
 * @param frequency I2C frequency the device is operating on
 * @param deviceAddress I2C Device address (7Bit)
 */
template<class registerAddressType, Endians::ByteOrder registerAddressEndianess>
IO::I2C::Device<registerAddressType, registerAddressEndianess>::Device(
    Bus&      busInstance,
    uint8_t   deviceAddress,
    Frequency frequency)
        : bus {busInstance}, frequency {frequency}, deviceAddress {
                                                        deviceAddress}
{
    static_assert(std::is_same<registerAddressType, uint8_t>::value ||
                      std::is_same<registerAddressType, uint16_t>::value,
                  "only uint8_t and iuint16_t allowed as registerAdrressType");
}

/**
 * @brief Set given registers of the devices.
 * 
 * @tparam registerAddressType uint8_t or uint16_t
 * @tparam registerAddressEndianess Only for uint16_t: endianess of the registerAdresses
 * @param values Data pointer to the values to write.
 * @param length Length of the data field.
 * @param registerAddress Address if the first register to write to.
 * @param timeout Maximum time to use for operation.
 * @return Error::Code Might Timeout or not reach partner.
 */
template<class registerAddressType, Endians::ByteOrder registerAddressEndianess>
Error::Code
    IO::I2C::Device<registerAddressType, registerAddressEndianess>::setRegisters(
        uint8_t*            values,
        size_t              length,
        registerAddressType registerAddress,
        RTOS::milliseconds  timeout)
{
    if (registerAddressEndianess == Endians::big) {
        Endians::machineToBig(registerAddress);
    } else if (registerAddressEndianess == Endians::little) {
        Endians::machineToLittle(registerAddress);
    } else {
        // must never happen
        return Error::Unknown;
    }

    return bus.setRegisters(frequency,
                            deviceAddress,
                            reinterpret_cast<uint8_t*>(&registerAddress),
                            sizeof(registerAddress),
                            values,
                            length,
                            timeout);
}

/**
 * @brief Set given registers of the devices.
 * 
 * @tparam registerAddressType uint8_t or uint16_t
 * @tparam registerAddressEndianess Only for uint16_t: endianess of the registerAdresses
 * @param values Data pointer to the data buffer to read to.
 * @param length Length of the data buffer.
 * @param registerAddress Address if the first register to read from.
 * @param timeout Maximum time to use for operation.
 * @return Error::Code Might Timeout or not reach partner.
 */
template<class registerAddressType, Endians::ByteOrder registerAddressEndianess>
Error::Code
    IO::I2C::Device<registerAddressType, registerAddressEndianess>::getRegisters(
        uint8_t*            values,
        size_t              length,
        registerAddressType registerAddress,
        RTOS::milliseconds  timeout)
{
    if (registerAddressEndianess == Endians::big) {
        Endians::machineToBig(registerAddress);
    } else if (registerAddressEndianess == Endians::little) {
        Endians::machineToLittle(registerAddress);
    } else {
        // must never happen
        return Error::Unknown;
    }

    return bus.getRegisters(frequency,
                            deviceAddress,
                            reinterpret_cast<uint8_t*>(&registerAddress),
                            sizeof(registerAddress),
                            values,
                            length,
                            timeout);
}

/**
 * @brief Get current I2C address of the device
 * 
 * @warning This driver uses 7-bit I2C addresses.
 * 
 * @return uint8_t current 7-bit I2C address of the device.
 */
template<class registerAddressType, Endians::ByteOrder registerAddressEndianess>
uint8_t IO::I2C::Device<registerAddressType,
                        registerAddressEndianess>::getDeviceAddress()
{
    return deviceAddress;
}

/**
 * @brief Set current I2C address of the device.
 * 
 * @warning This driver uses 7-bit I2C addresses.
 * 
 * @return None.
 */
template<class registerAddressType, Endians::ByteOrder registerAddressEndianess>
void IO::I2C::Device<registerAddressType,
                     registerAddressEndianess>::setDeviceAddress(uint8_t address)
{
    deviceAddress = address;
}

#endif
