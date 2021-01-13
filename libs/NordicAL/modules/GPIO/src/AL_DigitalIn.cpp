/**
 * @file AL_DigitalIn.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief implementation of class for digital input (gpio) management
 * @version 1.0
 * @date 2020-06-12
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include <AL_DigitalIn.h>
#include <nrf_gpio.h>
#include <nrf_gpiote.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

IO::DigitalIn::DigitalIn(uint32_t pin, Pull pull, bool isInverted)
    : GPIO(pin), isInverted(isInverted)
{
    nrf_gpio_cfg_input(pin, static_cast<nrf_gpio_pin_pull_t>(pull));
}

//------------------------------ DESTRUCTOR ----------------------------------

IO::DigitalIn::~DigitalIn() {}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

bool IO::DigitalIn::read()
{
    return isInverted != static_cast<bool>(nrf_gpio_pin_read(getPinNumber()));
}
