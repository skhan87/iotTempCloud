/**
 * @file InterruptIn.cpp
 * @author Petar Jukica (petar@aconno.de)
 * @brief abstract class which represents GPIO interrupts
 * @version 1.0
 * @date 2020-06-24
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include <AL_InterruptIn.h>
#include <PortUtility.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a interrupt in instance
 * 
 * @warning Callback is ISR. Implement accordingly!
 * 
 * @param pin Pin number to listen to
 * @param callback Callback to call. Is an ISR!
 * @param pull Switch for Pullup/downs
 * @param isInverted Whether the logic levels of the interrupt and the DigitalIn should be inverted
 * @param polarity Which type of signals trigger this interrupt, rising, falling or both
 */
IO::InterruptIn::InterruptIn(uint32_t          pin,
                             InterruptCallback callback,
                             IO::Pull          pull,
                             bool              isInverted,
                             Polarity          polarity)
        : DigitalIn(pin, pull, isInverted)
{
    init();

    // Setting up configuration structure
    nrfx_gpiote_in_config_t interruptConfig = {
        .sense           = static_cast<nrf_gpiote_polarity_t>(polarity),
        .pull            = static_cast<nrf_gpio_pin_pull_t>(pull),
        .is_watcher      = false,
        .hi_accuracy     = false,
        .skip_gpio_setup = true};

    // Binding pin interrupt with interruptMapper function
    auto errCode = nrfx_gpiote_in_init(
        static_cast<nrfx_gpiote_pin_t>(getPinNumber()),
        &interruptConfig,
        reinterpret_cast<nrfx_gpiote_evt_handler_t>(callback));

    // Check if the error occurred
    CHECK_ERROR(Port::Utility::getError(errCode));
    nrfx_gpiote_in_event_enable(getPinNumber(), true);
}

//------------------------------ DESTRUCTOR ----------------------------------

IO::InterruptIn::~InterruptIn()
{
    nrfx_gpiote_in_uninit(getPinNumber());
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Enabling GPIO Interrupt.
 *
 */
void IO::InterruptIn::enable()
{
    nrfx_gpiote_in_event_enable(static_cast<nrfx_gpiote_pin_t>(getPinNumber()),
                                true);
}

/**
 * @brief Disabling GPIO Interrupt.
 *
 */
void IO::InterruptIn::disable()
{
    nrfx_gpiote_in_event_disable(
        static_cast<nrfx_gpiote_pin_t>(getPinNumber()));
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief initializes the interrupt module.
 * 
 * @warning is used for lazy init from within lib only.
 * 
 */
void IO::InterruptIn::init()
{
    if (!nrfx_gpiote_is_init()) {
        auto errCode = nrfx_gpiote_init();
        CHECK_ERROR(Port::Utility::getError(errCode));
    }
}