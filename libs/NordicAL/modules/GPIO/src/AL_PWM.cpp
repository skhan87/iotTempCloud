/**
 * @file PWM.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for hardware implemented PWM on nrf52
 * @version 1.0
 * @date 2020-04-23
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#include <sdk_config.h>

#if NRFX_PWM_ENABLED
//--------------------------------- INCLUDES ----------------------------------

    #include <AL_GPIO.h>
    #include <AL_PWM.h>
    #include <PortUtility.h>
    #include <limits>
    #include <nrf_gpio.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

/**
 * @brief internal struct for holding information regarding PWM instances
 * that needs to be in RAM not on stack.
 *
 */
struct Instance {
    /** data needed by nordic driver */
    const nrfx_pwm_t nrfxData;
    /** series of counter values on which to toggle PWM output. */
    nrf_pwm_values_common_t value;
    /** whether this instance is already used*/
    bool used;
};

/**
 * @brief all available hardware PWM instances
 *
 */
static Instance instances[] = {
    #if NRFX_PWM0_ENABLED
    {
        .nrfxData = NRFX_PWM_INSTANCE(0),
        .value    = 0,
        .used     = false,
    },
    #endif
    #if NRFX_PWM1_ENABLED
    {
        .nrfxData = NRFX_PWM_INSTANCE(1),
        .value    = 0,
        .used     = false,
    },
    #endif
    #if NRFX_PWM2_ENABLED
    {
        .nrfxData = NRFX_PWM_INSTANCE(2),
        .value    = 0,
        .used     = false,
    },
    #endif
};

    #if !NRFX_PWM0_ENABLED && !NRFX_PWM1_ENABLED && !NRFX_PWM2_ENABLED
        #error no pwm active
    #endif

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

IO::PWM::PWM(uint8_t pin, float frequency, float dutyCycle, bool inverted)
    : pin(pin), inverted(inverted)
{
    // search free PWM channel
    for (index = 0; index < sizeof(instances) / sizeof(instances[0]); index++) {
        if (!instances[index].used) {
            CHECK_ERROR(getClock(frequency, clk, top));

            // found free channel, set it up
            instances[index].used = true;
            auto config           = getConfig();

            CHECK_ERROR(Port::Utility::getError(
                nrfx_pwm_init(&instances[index].nrfxData, &config, nullptr)));
            CHECK_ERROR(setDutyCycle(dutyCycle));

            // successfully initialized
            return;
        }
    }

    // no free channels, application dev uses too many PWM channels
    // at the same time. active for PWM channels via
    // NRFX_PWMx_ENABLED or use less. CRITICAL
    CHECK_ERROR(Error::OutOfResources);
}

IO::PWM::~PWM()
{
    nrfx_pwm_uninit(&instances[index].nrfxData);
    /// drive pin to safe state
    nrf_gpio_cfg_input(pin,
                       static_cast<nrf_gpio_pin_pull_t>(IO::Pull::Disabled));
}

//--------------------------- EXPOSED FUNCTIONS
//-------------------------------

Error::Code IO::PWM::enable()
{
    bool wasStopped = nrfx_pwm_is_stopped(&instances[index].nrfxData);
    if (!wasStopped) {
        // is already running
        return Error::None;
    }

    // reset from tristate to driving state
    nrf_gpio_pin_dir_set(pin, NRF_GPIO_PIN_DIR_OUTPUT);

    nrf_pwm_sequence_t sequence;
    sequence.values.p_common = &instances[index].value;
    // lib is built to be able to play sequences,
    // we just have one value to play (PWM).
    sequence.length          = 1;
    sequence.repeats         = 0;
    sequence.end_delay       = 0;

    // does have a return type, but it is a task id for a different
    // mode, not a error value
    nrfx_pwm_simple_playback(&instances[index].nrfxData,
                             &sequence,
                             1,
                             NRFX_PWM_FLAG_LOOP |
                                 NRFX_PWM_FLAG_NO_EVT_FINISHED);

    return Error::None;
}

Error::Code IO::PWM::disable()
{
    // function is async, so return value is current state.
    nrfx_pwm_stop(&instances[index].nrfxData, false);
    nrf_gpio_pin_dir_set(pin, NRF_GPIO_PIN_DIR_INPUT);
    return Error::None;
}

float IO::PWM::getFrequency()
{
    return baseClockToHertz(clk) / top;
}

Error::Code IO::PWM::setFrequency(float frequency)
{
    auto result = getClock(frequency, clk, top);
    if (result != Error::None) {
        return result;
    }

    bool wasStopped = nrfx_pwm_is_stopped(&instances[index].nrfxData);

    // stop pwm and uninit
    nrfx_pwm_uninit(&instances[index].nrfxData);

    auto config = getConfig();
    if (nrfx_pwm_init(&instances[index].nrfxData, &config, nullptr) !=
        NRFX_SUCCESS) {
        // failed to init PWM
        return Error::Unknown;
    }

    if (!wasStopped) {
        return enable();
    } else {
        return Error::None;
    }
}

float IO::PWM::getDutyCycle()
{
    float value =
        static_cast<float>(instances[index].value) / static_cast<float>(top);
    if (inverted) {
        return 1.0f - value;
    } else {
        return value;
    }
}

Error::Code IO::PWM::setDutyCycle(float dutyCycle)
{
    if (dutyCycle < 0.0f || dutyCycle > 1.0f) {
        return Error::InvalidParameter;
    }

    // handle invert here, since nordics driver invert does not work
    if (inverted) {
        dutyCycle = 1.0f - dutyCycle;
    }

    instances[index].value =
        static_cast<uint16_t>(dutyCycle * static_cast<float>(top));

    nrf_pwm_values_t newVals;
    newVals.p_common = &instances[index].value;
    nrfx_pwm_sequence_values_update(&instances[index].nrfxData, 0, newVals);
    nrfx_pwm_sequence_values_update(&instances[index].nrfxData, 1, newVals);
    return Error::None;
}

//----------------------- INTERFACE IMPLEMENTATIONS
//---------------------------

//--------------------------- PRIVATE FUNCTIONS
//-------------------------------

nrfx_pwm_config_t IO::PWM::getConfig()
{
    // do not use nordic invert!
    // It does work on init, but not while running.
    return {
        .output_pins  = {pin,
                        NRFX_PWM_PIN_NOT_USED,
                        NRFX_PWM_PIN_NOT_USED,
                        NRFX_PWM_PIN_NOT_USED},
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock   = clk,
        .count_mode   = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
        .top_value    = top,
        .load_mode    = (nrf_pwm_dec_load_t)NRFX_PWM_DEFAULT_CONFIG_LOAD_MODE,
        .step_mode    = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,
    };
}

//---------------------------- STATIC FUNCTIONS
//-------------------------------

Error::Code IO::PWM::getClock(float          frequency,
                              nrf_pwm_clk_t& clock,
                              uint16_t&      top)
{
    for (int i = (int)NRF_PWM_CLK_16MHz; i <= (int)NRF_PWM_CLK_125kHz; ++i) {
        float baseClk = baseClockToHertz((nrf_pwm_clk_t)i);
        if ((baseClk / kMaxTop) <= frequency) {
            clock = (nrf_pwm_clk_t)i;
            top   = (uint16_t)(baseClk / frequency);
            if (top > kMinResolution) {
                return Error::None;
            }
        }
    }
    // resolution of the PWM would not satisfy kMinResolution
    return Error::MinReached;
}

float IO::PWM::baseClockToHertz(nrf_pwm_clk_t clock)
{
    switch (clock) {
        case NRF_PWM_CLK_16MHz:
            return 16e6;
        case NRF_PWM_CLK_8MHz:
            return 8e6;
        case NRF_PWM_CLK_4MHz:
            return 4e6;
        case NRF_PWM_CLK_2MHz:
            return 2e6;
        case NRF_PWM_CLK_1MHz:
            return 1e6;
        case NRF_PWM_CLK_500kHz:
            return 500e3;
        case NRF_PWM_CLK_250kHz:
            return 250e3;
        case NRF_PWM_CLK_125kHz:
            return 125e3;
        default:
            // invalid value, should never happen
            CHECK_ERROR(Error::Unknown);
            return -1.0;
            break;
    }
}
#endif