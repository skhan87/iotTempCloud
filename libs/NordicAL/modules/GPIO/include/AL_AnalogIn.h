/**
 * @file AL_AnalogIn.h
 * @author Petar Jukica (petar@aconno.de)
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief implementation of class for analog input.
 * @version 1.1
 * @date 2020-09-01
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_ANALOGIN_H__
#define __AL_ANALOGIN_H__

//--------------------------------- INCLUDES ----------------------------------

#include "AL_GPIO.h"
#include "nrfx_saadc.h"

#include <Error.h>
#include <array>

namespace IO
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

class AnalogIn {
    // Delete default constructors
    AnalogIn(const AnalogIn& other) = delete;
    AnalogIn& operator=(const AnalogIn& other) = delete;

private:
    enum class Resolution {
        Bit8  = NRF_SAADC_RESOLUTION_8BIT,   // 8 bit resolution.
        Bit10 = NRF_SAADC_RESOLUTION_10BIT,  // 10 bit resolution.
        Bit12 = NRF_SAADC_RESOLUTION_12BIT,  // 12 bit resolution.
        Bit14 = NRF_SAADC_RESOLUTION_14BIT   // 14 bit resolution.
    };

    enum class OversamplingFactor {
        F0   = 0,  // Oversampling disabled
        F2   = 1,  // Oversampling rate 2x
        F4   = 2,  // Oversampling rate 4x
        F8   = 3,  // Oversampling rate 8x
        F16  = 4,  // Oversampling rate 16x
        F32  = 5,  // Oversampling rate 32x
        F64  = 6,  // Oversampling rate 64x
        F128 = 7,  // Oversampling rate 128x
        F256 = 8   // Oversampling rate 256x
    };

    struct ChannelData {
        nrf_saadc_channel_config_t
                nrfConfig; /**< configuration of the channel */
        bool    used;      /**< whether this channel is currently used */
        uint8_t number;    /**< number oof the channel */
    };

public:
    AnalogIn(GPIO& measurePin);
    AnalogIn();

    ~AnalogIn();

    Error::Code read(int16_t& value);

private:
    ChannelData& channel;
    GPIO*        measurePin;

    /**
     * @brief Called on A/D conversion event.
     *
     * @param event
     */
    static void eventHandler(nrfx_saadc_evt_t const* event);
    static bool initialized;
    static void init();

    /**
     * @brief number of ADC channel available
     * 
     */
    static constexpr uint8_t CHANNEL_COUNT = 8;

    static constexpr nrf_saadc_input_t getAnalogInFromPin(uint32_t pin)
    {
#if defined(NRF52832_XXAA)
        switch (pin) {
            case pinMap(0, 2):
                return NRF_SAADC_INPUT_AIN0;
            case pinMap(0, 3):
                return NRF_SAADC_INPUT_AIN1;
            case pinMap(0, 4):
                return NRF_SAADC_INPUT_AIN2;
            case pinMap(0, 5):
                return NRF_SAADC_INPUT_AIN3;
            case pinMap(0, 28):
                return NRF_SAADC_INPUT_AIN4;
            case pinMap(0, 29):
                return NRF_SAADC_INPUT_AIN5;
            case pinMap(0, 30):
                return NRF_SAADC_INPUT_AIN6;
            case pinMap(0, 31):
                return NRF_SAADC_INPUT_AIN7;
            default:
                return NRF_SAADC_INPUT_DISABLED;
        }
#elif defined(NRF52840_XXAA)
        switch (pin) {
            case pinMap(0, 2):
                return NRF_SAADC_INPUT_AIN0;
            case pinMap(0, 3):
                return NRF_SAADC_INPUT_AIN1;
            case pinMap(0, 4):
                return NRF_SAADC_INPUT_AIN2;
            case pinMap(0, 5):
                return NRF_SAADC_INPUT_AIN3;
            case pinMap(0, 28):
                return NRF_SAADC_INPUT_AIN4;
            case pinMap(0, 29):
                return NRF_SAADC_INPUT_AIN5;
            case pinMap(0, 30):
                return NRF_SAADC_INPUT_AIN6;
            case pinMap(0, 31):
                return NRF_SAADC_INPUT_AIN7;
            default:
                return NRF_SAADC_INPUT_DISABLED;
        }
#else
    #error port not implemented
#endif
    }
    static std::array<ChannelData, CHANNEL_COUNT>& getChannels();
    static ChannelData&                            allocChannel();
    static void freeChannel(ChannelData& channel);
};

}  // namespace IO

#endif  // __AL_ANALOGIN_H__