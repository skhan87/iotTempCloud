/**
 * @file AL_I2CBus.h
 * @author Petar Jukica (petar@aconno.de)
 * @brief I2C bus implementation
 * @version 0.1
 * @date 2020-09-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <sdk_config.h>
#include "nrfx_common.h"

#if NRFX_TWIM_ENABLED

// guard against stupid config
#if !NRFX_CHECK(NRFX_TWIM0_ENABLED) && !NRFX_CHECK(NRFX_TWIM1_ENABLED)
#error You need to enable at least on TWIM instance or disable the driver
#endif

#ifndef __AL_BUSI2C_H__
#define __AL_BUSI2C_H__

#include <AL_RTOS.h>
#include <AL_Event.h>
#include <AL_EventGroup.h>
#include <AL_Mutex.h>
#include <Error.h>
#include <cstdint>
#include <nrfx_twim.h>
#include <string.h>
#include <memory>
#include <ScopeExit.h>
#include <AL_Log.h>

namespace IO::I2C
{
/**
 * @brief Available I2C frequencies
 * 
 */
enum class Frequency {
    _100K = NRF_TWIM_FREQ_100K,
    _250K = NRF_TWIM_FREQ_250K,
    _400K = NRF_TWIM_FREQ_400K
};

/**
 * @brief Currently enabled TWIM instances
 * 
 */
enum class BusInstance {
#if NRFX_CHECK(NRFX_TWIM0_ENABLED)
    TWIM0 = NRFX_TWIM0_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_TWIM1_ENABLED)
    TWIM1 = NRFX_TWIM1_INST_IDX,
#endif
    // Used to determine how many instances are currently available
    NUM_OF_TWIM_INSTANCES

};

/**
 * @brief I2C (TWI) master bus instance.
 * 
 * @details Create a I2C bus on pins to then attach i2c device to it.
 * Is implemented with a DMA driver.
 * 
 */
class Bus {
    Bus()                 = delete;
    Bus(const Bus& other) = delete;
    Bus& operator=(const Bus& other) = delete;

public:
    Bus(uint32_t scl,
        uint32_t sda,
        uint8_t  interruptPriority = NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY);

    Error::Code setRegisters(Frequency          frequency,
                             uint8_t            deviceAddress,
                             const uint8_t*     registerAddress,
                             size_t             registerAddressSize,
                             uint8_t*           data,
                             size_t             size,
                             RTOS::milliseconds timeout = RTOS::Infinity);

    Error::Code getRegisters(Frequency          frequency,
                             uint8_t            deviceAddress,
                             const uint8_t*     registerAddress,
                             size_t             registerAddressSize,
                             uint8_t*           data,
                             size_t             size,
                             RTOS::milliseconds timeout = RTOS::Infinity);

private:
    void errata89Workaround();

    /**
     * @brief Data associated with each instance of the nrfx driver.
     * 
     */
    struct nrfxInstanceData {
        bool allocated; /**< whether this nrfxInstance is alredy in use */
        nrfx_twim_t
                           twimInstance; /**< I2C instance that is used for data transfer. */
        nrfx_twim_config_t config; /**< configuration of the I2C instance. */
    };

    static nrfxInstanceData* allocInstance();
    static void              onTransferComplete(nrfx_twim_evt_t const* p_event,
                                                void*                  p_context);

    static nrfxInstanceData nrfxInstances[];

    nrfxInstanceData&
                nrfxInstance; /**< instance date needed for nrfx module. Will be one of the nrfxInstances fields. */
    RTOS::Mutex transferLock; /**<  Used for mutual exclusion of the transfers 
                                       *   running on the same I2C instance. */
    RTOS::EventGroup
        events; /**< Container for the transfer complete event bit. */
    RTOS::Event
        transferComplete; /**< Event used for signaling transfer completion. */
};
}  // namespace IO::I2C

#endif
#endif