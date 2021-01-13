/**
 * @file AL_GPIO.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief class for digital output (gpio) management
 * @version 1.1
 * @date
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_GPIO_H__
#define __AL_GPIO_H__

//--------------------------------- INCLUDES ----------------------------------

#include <cstdint>
#include <nrf_gpio.h>

namespace IO
{
//--------------------------- STRUCTS AND ENUMS -------------------------------

/**
 * @brief Input pin pull.
 *
 *
 */
enum class Pull {
    Down     = NRF_GPIO_PIN_PULLDOWN,
    Up       = NRF_GPIO_PIN_PULLUP,
    Disabled = NRF_GPIO_PIN_NOPULL
};

//-------------------------------- PROTOTYPES ---------------------------------

//-------------------------------- CONSTANTS ----------------------------------

/**
 * @brief Maps port and pin values to a single pin number.
 *
 * @return Pin number that coresponds to port and pin values.
 */
constexpr uint32_t pinMap(uint8_t port, uint32_t pin)
{
    return (port << 5) | (pin & 0x1F);
}

//---------------------------- CLASS DEFINITION -------------------------------

class GPIO {
public:
    // Constructors

    // Delete default constructors
    GPIO()                  = delete;
    GPIO(const GPIO& other) = delete;
    GPIO& operator=(const GPIO& other) = delete;

    constexpr GPIO(uint32_t pin) : pinNumber{pin} {}

    constexpr uint32_t getPinNumber() { return pinNumber; }

private:
    const uint32_t pinNumber;
};
}  // namespace IO

#endif  // __A_GPIO_H__