/**
 * @file AL_DigitalOut.cpp
 * @author Petar Jukica (petar@aconno.de)
 * @brief class for digital output (gpio) management
 * @version 1.0
 * @date 2020-06-12
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include <AL_DigitalIn.h>
#include <AL_DigitalOut.h>
#include <nrf_gpio.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief DigitalOut object constructor.
 * 
 * @param pin   Single pin number. Use IO::pinMap() to obtain this from more
 *              common port/pin definition.
 * @param isInverted    Boolean which determines whether pin values should be
 *                      inverted or not, i.e. in case when device enable pin 
 *                      is active low, inverting results in switchOn() turning
 *                      device on, which makes more sense.
 * @param initialState  Boolean which determines whether output pin initial state
 *                      will be high or low. Defaults to low.
 */
IO::DigitalOut::DigitalOut(uint32_t  pin,
                           bool      isInverted,
                           bool      initialState,
                           DriveMode mode)
        : GPIO(pin), isInverted(isInverted)
{
    nrf_gpio_cfg(getPinNumber(),
                 NRF_GPIO_PIN_DIR_OUTPUT,
                 NRF_GPIO_PIN_INPUT_DISCONNECT,
                 NRF_GPIO_PIN_NOPULL,
                 static_cast<nrf_gpio_pin_drive_t>(mode),
                 NRF_GPIO_PIN_NOSENSE);
    DigitalOut::set(initialState);
}

//------------------------------ DESTRUCTOR -----------------------------------
/**
 * @brief Destroy the IO::DigitalOut::DigitalOut object
 * 
 * @details Digital output pin is being reconfigured to a digital input with 
 *          pull resistors disabled. 
 */
IO::DigitalOut::~DigitalOut()
{
    nrf_gpio_cfg_input(getPinNumber(),
                       static_cast<nrf_gpio_pin_pull_t>(IO::Pull::Disabled));
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Toggle power level of a pin.
 * 
 * @return None.
 */
void IO::DigitalOut::toggle()
{
    nrf_gpio_pin_toggle(getPinNumber());
}

/**
 * @brief Change power level of a pin to low.
 *
 * @return None.
 */
void IO::DigitalOut::switchOff()
{
    if (isInverted) {
        nrf_gpio_pin_write(getPinNumber(), kHigh);

    } else {
        nrf_gpio_pin_write(getPinNumber(), kLow);
    }
}

/**
 * @brief Change power level of a pin to high.
 *
 * @return None.
 */
void IO::DigitalOut::switchOn()
{
    if (isInverted) {
        nrf_gpio_pin_write(getPinNumber(), kLow);

    } else {
        nrf_gpio_pin_write(getPinNumber(), kHigh);
    }
}

/**
 * @brief Sets power level of a pin.
 *
 * @return None.
 */
void IO::DigitalOut::set(bool value)
{
    if (isInverted) {
        nrf_gpio_pin_write(getPinNumber(), !value);
    } else {
        nrf_gpio_pin_write(getPinNumber(), value);
    }
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
