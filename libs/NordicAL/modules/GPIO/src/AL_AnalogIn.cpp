/**
 * @file AL_AnalogIn.cpp
 * @author Petar Jukica (petar@aconno.de)
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief implementation of class for analog input.
 * @version 1.1
 * @date 2020-09-01
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_AnalogIn.h"

#include "AL_Log.h"
#include "PortUtility.h"
#include "nrfx_saadc.h"

#include <limits>

//--------------------------- STRUCTS AND ENUMS -------------------------------

bool IO::AnalogIn::initialized = false;

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Construct AnalogIn on given pin
 * 
 * @param pin 
 */
IO::AnalogIn::AnalogIn(GPIO& measurePin)
    : channel(allocChannel()), measurePin(&measurePin)
{
    auto analogIn = getAnalogInFromPin(measurePin.getPinNumber());
    if (analogIn == NRF_SAADC_INPUT_DISABLED) {
        // given pin can not be used as AnalogIn
        CHECK_ERROR(Error::InvalidUse);
    }

    channel.nrfConfig = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(analogIn);

    // Channel initializing
    auto errCode = nrfx_saadc_channel_init(channel.number, &channel.nrfConfig);
    CHECK_ERROR(Port::Utility::getError(errCode));
}

/**
 * @brief Construct AnalogIn on supply voltage
 * 
 */
IO::AnalogIn::AnalogIn() : channel(allocChannel()), measurePin(nullptr)
{
    channel.nrfConfig =
        NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);

    // Channel initializing
    auto errCode = nrfx_saadc_channel_init(channel.number, &channel.nrfConfig);
    CHECK_ERROR(Port::Utility::getError(errCode));
}

IO::AnalogIn::~AnalogIn()
{
    nrfx_saadc_channel_uninit(channel.number);
    freeChannel(channel);
}

/**
 * @brief Start A/D conversion, read and
 * return measurement.
 *
 * @return A/D conversion (sampling) return value
 */
Error::Code IO::AnalogIn::read(int16_t& value)
{
    return ::Port::Utility::getError(nrfx_saadc_sample_convert(0, &value));
}

//---------------------------- STATIC FUNCTIONS -------------------------------

void IO::AnalogIn::eventHandler(nrfx_saadc_evt_t const* event) {}

/**
 * @brief called by Port::init.
 * 
 * @warning do not call from application code.
 * 
 */
void IO::AnalogIn::init()
{
    // Configuring A/D conversion parameters
    const nrfx_saadc_config_t saadcConfig =
        {.resolution =
             static_cast<nrf_saadc_resolution_t>(NRFX_SAADC_CONFIG_RESOLUTION),
         .oversample =
             static_cast<nrf_saadc_oversample_t>(NRFX_SAADC_CONFIG_OVERSAMPLE),
         .interrupt_priority = NRFX_SAADC_CONFIG_IRQ_PRIORITY,
         .low_power_mode     = NRFX_SAADC_CONFIG_LP_MODE};

    auto errCode = nrfx_saadc_init(&saadcConfig, IO::AnalogIn::eventHandler);
    CHECK_ERROR(Port::Utility::getError(errCode));

    auto channels = getChannels();
    if (channels.size() >
        std::numeric_limits<typeof(ChannelData::number)>::max()) {
        // too many channels used
        CHECK_ERROR(Error::OutOfResources);
    }

    for (size_t i = 0; i < channels.size(); i++) {
        channels[i].used   = false;
        channels[i].number = static_cast<typeof(ChannelData::number)>(i);
    }

    for (auto& channel : channels) {
        channel.used = false;
    }
    initialized = true;
}

/**
 * @brief contains all configuration and allocation information.
 * 
 * @details lazy loading for proper order of construction.
 * 
 */
std::array<IO::AnalogIn::ChannelData, IO::AnalogIn::CHANNEL_COUNT>& IO::
    AnalogIn::getChannels()
{
    static std::array<ChannelData, CHANNEL_COUNT> channels;
    return channels;
}

/**
 * @brief allocates a channel in the array
 * 
 * @return IO::AnalogIn::ChannelData& 
 */
IO::AnalogIn::ChannelData& IO::AnalogIn::allocChannel()
{
    // lazy initalize module here when needed
    if (!initialized) {
        init();
    }

    for (auto& channel : getChannels()) {
        if (!channel.used) {
            channel.used = true;
            return channel;
        }
    }

    CHECK_ERROR(Error::OutOfResources);
    // never gets here, but can not return nothing
    return getChannels()[0];
}

/**
 * @brief frees a channel
 * 
 * @param channel 
 */
void IO::AnalogIn::freeChannel(ChannelData& channel)
{
    if (channel.used) {
        channel.used = false;
    } else {
        CHECK_ERROR(Error::InvalidUse);
    }
}