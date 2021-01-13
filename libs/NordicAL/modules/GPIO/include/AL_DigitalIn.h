/**
 * @file AL_DigitalIn.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief class for digital input (gpio) management
 * @version 1.0
 * @date 2020-06-12
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_DIGITALIN_H__
#define __AL_DIGITALIN_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <AL_GPIO.h>
#include <cstdint>
#include <nrf_gpio.h>

namespace IO
{
//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

class DigitalIn : public IO::GPIO {
public:
    // Constructors

    // Delete default constructors
    DigitalIn()                       = delete;
    DigitalIn(const DigitalIn& other) = delete;
    DigitalIn& operator=(const DigitalIn& other) = delete;

    DigitalIn(uint32_t pin,
              Pull     pull       = Pull::Disabled,
              bool     isInverted = false);

    // Destructor
    ~DigitalIn();

    // exposed functions

    /**
     * @brief Returns power level of a pin.
     *
     * @return  true for HIGH
     * @return  false for LOW
     */
    bool read();

private:
    /**
     * @brief Boolean which indicates whether pin uses
     * inverted logic or not.
     *
     */
    bool isInverted;
};

}  // namespace IO

#endif  // __AL_DIGITALIN_H__