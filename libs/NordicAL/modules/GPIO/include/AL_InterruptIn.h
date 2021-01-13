/**
 * @file InterruptIn.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief Class that encapsulates GPIO (input) interrupt functionality
 * @version 1.0
 * @date 2020-06-21
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_INTERRUPTIN_H__
#define __AL_INTERRUPTIN_H__

//--------------------------------- INCLUDES ----------------------------------

#include <AL_DigitalIn.h>
#include <LifetimeList.h>
#include <array>
#include <nrfx.h>
#include <nrfx_gpiote.h>

namespace IO
{
//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------- STRUCTS AND ENUMS -------------------------------

/**
 * @brief Types of signal transitions that can cause a pin interrupt.
 *
 *
 */
enum class Polarity {
    RisingEdge  = nrf_gpiote_polarity_t::NRF_GPIOTE_POLARITY_LOTOHI,
    FallingEdge = nrf_gpiote_polarity_t::NRF_GPIOTE_POLARITY_HITOLO,
    OnToggle    = nrf_gpiote_polarity_t::NRF_GPIOTE_POLARITY_TOGGLE
};

/**
 * @brief Prototype for interrupt callbacks that can be hooked.
 * 
 * @param pin Pin number that triggered the interrupt.
 * @param action Action which triggered the interrupt (Either Rising or Falling).
 * 
 */
using InterruptCallback = void (*)(uint32_t pin, Polarity action);

//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Initializes a pin as DigitalIn and attaches an interrupt.
 * 
 * @example To use this, simply instantiate it
 * and pass a ISR to call on interrupt:
 * 
 * ```cpp
 * IO::DigitalOut pin2{21};
 * 
 * void isr(uint32_t pin, Polarity action) {
 *     pin2.toggle();
 * }
 * 
 * InterruptIn interupt1{20, &isr};
 * ```
 */
class InterruptIn : public IO::DigitalIn {
public:
    // Constructors

    // Delete default constructors
    InterruptIn()                         = delete;
    InterruptIn(const InterruptIn& other) = delete;
    InterruptIn& operator=(const InterruptIn& other) = delete;

    InterruptIn(uint32_t          pin,
                InterruptCallback callback,
                IO::Pull          pull       = Pull::Disabled,
                bool              isInverted = false,
                Polarity          polarity   = Polarity::OnToggle);

    ~InterruptIn();

    void enable();
    void disable();

private:
    static void init();
};

}  // namespace IO

#endif  //__AL_INTERRUPTIN_H