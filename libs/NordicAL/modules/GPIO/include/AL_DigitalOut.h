/**
 * @file AL_DigitalOut.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief class for digital output (gpio) management
 * @version 1.0
 * @date 2020-06-12
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_DIGITALOUT_H__
#define __AL_DIGITALOUT_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------
#include <AL_GPIO.h>
#include <cstdint>
#include <nrf_gpio.h>

namespace IO
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
/**
 * @brief Implements digital output pin object.
 * 
 * @example DigitalOut usage:
 * ```cpp
 *      // Defines pin P0.08 (port 0, pin 8) as digital output,
 *      // not inverted, initially set to LOW
 *      IO::DigitalOut pullUpPin(IO::pinMap(0, 8));
 * 
 *      // Defines pin P1.02 (port 1, pin 2) as digital output,
 *      // inverted, initially set to HIGH
 *      IO::DigitalOut enablePin(IO::pinMap(1, 2), true, true);
 * 
 *      pullUpPin.switchOn();
 *      enablePin.switchOff();
 * 
 *      pullUpPin.set(false);
 * ```
 */
class DigitalOut : public IO::GPIO {
public:
    enum class DriveMode {
        Standard = NRF_GPIO_PIN_S0S1, /**<  Standard '0', standard '1'. */
        High0Standard1 =
            NRF_GPIO_PIN_H0S1, /**< High - drive '0', standard '1' */
        Standard0High1 =
            NRF_GPIO_PIN_S0H1, /**< Standard '0', high - drive '1'. */
        High0High1 =
            NRF_GPIO_PIN_H0H1, /**< High drive '0', high - drive '1'. */
        Disconnect0Standard1 =
            NRF_GPIO_PIN_D0S1, /**< Disconnect '0' standard '1'. */
        Disconnect0High1 =
            NRF_GPIO_PIN_D0H1, /**< Disconnect '0', high - drive '1'. */
        Standard0Disconnect1 =
            NRF_GPIO_PIN_S0D1, /**< Standard '0', disconnect '1'. */
        High0Disconnect1 =
            NRF_GPIO_PIN_H0D1 /**< High - drive '0', disconnect '1' */
    };

    // Delete default constructors
    DigitalOut()                        = delete;
    DigitalOut(const DigitalOut& other) = delete;
    DigitalOut& operator=(const DigitalOut& other) = delete;

    /*--- Constructor ---*/
    DigitalOut(uint32_t  pin,
               bool      isInverted   = false,
               bool      initialState = false,
               DriveMode mode         = DriveMode::Standard);

    /*--- Destructor ---*/
    ~DigitalOut();

    /*--- Public API ---*/
    void toggle();
    void switchOn();
    void switchOff();
    void set(bool value);

private:
    /*--- Private members ---*/
    bool isInverted; /**< Indicates whether pin uses inverted logic or not. */

    /*--- Private constants ---*/
    constexpr static uint32_t kHigh = 1; /**< Default high value. */
    constexpr static uint32_t kLow  = 0; /**< Default low value. */
};

}  // namespace IO
#endif  //  __AL_DIGITALOUT_H__