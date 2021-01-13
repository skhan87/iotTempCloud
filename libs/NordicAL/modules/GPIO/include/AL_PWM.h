/**
 * @file PWM.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief pwm abstraction for hardware implemented nrf52 PWM
 * @version 1.0
 * @date 2020-04-23
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#include <sdk_config.h>

#if NRFX_PWM_ENABLED

    #ifndef __PWM_H__
        #define __PWM_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO
{
class PWM;
}

    //--------------------------------- INCLUDES
    //----------------------------------

        #include <nrfx_pwm.h>
        #include <Error.h>

namespace IO
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief pwm abstraction for hardware implemented nrf52 PWM
 */
class PWM
{
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    PWM()                 = delete;
    PWM(const PWM &other) = delete;
    PWM &operator=(const PWM &other) = delete;

    /**
     * @brief Construct a new PWM instance
     *
     * @param pin output pin of the PWM
     * @param frequency frequency in Hertz
     * @param dutyCycle duty cycle [0.0f-1.0f]
     * @param inverted PWm is inverted
     */
    PWM(uint8_t pin, float frequency, float dutyCycle, bool inverted = false);

    ~PWM();

    /**
     * @brief enable this pwm instance
     *
     */
    Error::Code enable();

    /**
     * @brief disable this pwm instance
     */
    Error::Code disable();

    /**
     * @brief Get the Frequency in Hertz
     *
     * @return float
     */
    float getFrequency();

    /**
     * @brief Set the Frequency in Hertz
     *
     * @param frequency
     * @return true success
     * @return false failure
     */
    Error::Code setFrequency(float frequency);

    /**
     * @brief Return the duty cycle [0.0-1.0].
     *
     * @return float
     */
    virtual float getDutyCycle();

    /**
     * @brief Set the duty cycle of a channel
     *
     * @param channel Channel to set duty cycle on
     * @param dutyCycle duty cycle [0.0-1.0]
     */
    virtual Error::Code setDutyCycle(float dutyCycle);

private:
    /** index of the pwm instance in the internal array */
    uint8_t index;
    /** currently used pin */
    uint8_t pin;
    /** top value for the counter */
    uint16_t top;
    /** base clock */
    nrf_pwm_clk_t clk;
    /** whether or not the PWM is inverted */
    bool inverted;

    /**
     * @brief Get the config struct for this instance.
     *
     * @return nrf_drv_pwm_config_t
     */
    nrfx_pwm_config_t getConfig();

    /**
     * @brief Get the Clock settings to achieve highest resolution while still
     * not overflowing
     *
     * @param frequency in Hertz
     * @param clock
     * @param top
     * @return true succeess
     * @return false failure
     */
    static Error::Code getClock(float frequency, nrf_pwm_clk_t &clock, uint16_t &top);

    /**
     * @brief converts base clock frequency to hertz
     *
     * @param clock
     * @return float
     */
    static float baseClockToHertz(nrf_pwm_clk_t clock);

    /** maximum top value of the PWM driver */
    static constexpr uint16_t kMaxTop = 1u << 15;
    /** minimum resolution a PWM should be allowed to have */
    static constexpr uint16_t kMinResolution = 128;
};
}  // namespace IO

#endif  //__PWM_H__

#endif