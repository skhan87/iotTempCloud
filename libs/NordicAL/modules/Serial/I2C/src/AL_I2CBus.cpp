/**
 * @file AL_I2CBus.cpp
 * @author Petar Jukica (petar@aconno.de)
 * @brief 
 * @version 0.1
 * @date 2020-09-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <sdk_config.h>

#if NRFX_TWIM_ENABLED

//--------------------------------- INCLUDES ----------------------------------

#include <AL_I2CBus.h>
#include <PortUtility.h>
#include <FunctionScopeTimer.h>

//--------------------------------- ARRAYS ------------------------------------

IO::I2C::Bus::nrfxInstanceData IO::I2C::Bus::nrfxInstances[] = {
#if NRFX_CHECK(NRFX_TWIM0_ENABLED)
    {.allocated = false, .twimInstance = NRFX_TWIM_INSTANCE(0), .config = {0}},
#endif
#if NRFX_CHECK(NRFX_TWIM1_ENABLED)
    {.allocated = false, .twimInstance = NRFX_TWIM_INSTANCE(1), .config = {0}},
#endif
};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Construct a new Bus object
 * 
 * @warning Do only use with static allocation. The constructor can throw.
 * 
 * @param scl Clock pin of the I2C bus
 * @param sda Data pin of the I2C bus
 * @param interruptPriority Priority of the interrupt that gets triggered on transfer completion
 * 
 */
IO::I2C::Bus::Bus(uint32_t scl, uint32_t sda, uint8_t interruptPriority)
        : nrfxInstance {*allocInstance()}, transferLock {}, events {},
          transferComplete {events}
{
    nrfxInstance.config = {
        .scl       = scl,
        .sda       = sda,
        .frequency = (nrf_twim_frequency_t)NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY,
        .interrupt_priority = interruptPriority,
        .hold_bus_uninit    = NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT};
}

/**
 * @brief Sets size number of registers in device.
 * 
 * @warning This method uses dynamic allocation.
 * 
 * @warning I2C transfers are blocking on RTOS events and
 * can therefor only be executed in task context.
 * 
 * @param frequency Frequency for the given transfer
 * @param deviceAddress I2C Slave address for the device
 * @param registerAddress Pointer to the register address
 * @param registerAddressSize Size of the register address in bytes
 * @param data Data to put into the registers
 * @param size Date size in bytes
 * @param timeout Maximum time to wait for transfert to finish
 * @return Error::Code Can return error on failure or timeout.
 */
Error::Code IO::I2C::Bus::setRegisters(Frequency          frequency,
                                       uint8_t            deviceAddress,
                                       const uint8_t*     registerAddress,
                                       size_t             registerAddressSize,
                                       uint8_t*           data,
                                       size_t             size,
                                       RTOS::milliseconds timeout)
{
    RTOS::FunctionScopeTimer timeoutTimer {timeout};

    RETURN_ON_ERROR(transferLock.tryObtain(timeout));
    // automatically release mutex when the function returns
    auto mutexRelease =
        Patterns::make_scopeExit(&RTOS::Mutex::tryRelease, transferLock);

    // Reconfiguring TWI master instance

    // Setting up device frequency
    nrfxInstance.config.frequency =
        static_cast<nrf_twim_frequency_t>(frequency);
    RETURN_ON_ERROR(
        Port::Utility::getError(nrfx_twim_init(&nrfxInstance.twimInstance,
                                               &nrfxInstance.config,
                                               &onTransferComplete,
                                               &transferComplete)));
    nrfx_twim_enable(&nrfxInstance.twimInstance);
    // make sure twimInstance is freed when the function exits
    auto twiDeinit = Patterns::make_scopeExit([this]() {
        nrfx_twim_disable(&nrfxInstance.twimInstance);
        nrfx_twim_uninit(&nrfxInstance.twimInstance);
        errata89Workaround();
        transferComplete.reset();
    });

    size_t                     bufferSize = registerAddressSize + size;
    std::unique_ptr<uint8_t[]> setupBuffer {new uint8_t[bufferSize]};

    // Setting up transfer buffer
    memcpy(setupBuffer.get(), registerAddress, registerAddressSize);
    memcpy(setupBuffer.get() + registerAddressSize, data, size);
    // Write to the device
    RETURN_ON_ERROR(
        Port::Utility::getError(nrfx_twim_tx(&nrfxInstance.twimInstance,
                                             deviceAddress,
                                             setupBuffer.get(),
                                             bufferSize,
                                             true)));

    // Blocking until the transfer is complete or timeout
    RETURN_ON_ERROR(transferComplete.await(timeoutTimer.timeLeft()));
    return Error::None;
}

/**
 * @brief Gets the size number of registers from device address.
 * 
 * @warning This method uses dynamic allocation.
 * 
 * @warning I2C transfers are blocking on RTOS events and
 * can therefor only be executed in task context.
 * 
 * @param frequency Frequency for the given transfer
 * @param deviceAddress I2C Slave address for the device
 * @param registerAddress Pointer to the register address
 * @param registerAddressSize Size of the register address in bytes
 * @param data Data to put into the registers
 * @param size Date size in bytes
 * @param timeout Maximum time to wait for transfert to finish
 * @return Error::Code Can return error on failure or timeout.
 */
Error::Code IO::I2C::Bus::getRegisters(Frequency          frequency,
                                       uint8_t            deviceAddress,
                                       const uint8_t*     registerAddress,
                                       size_t             registerAddressSize,
                                       uint8_t*           data,
                                       size_t             size,
                                       RTOS::milliseconds timeout)
{
    RTOS::FunctionScopeTimer timeoutTimer {timeout};
    RETURN_ON_ERROR(transferLock.tryObtain(timeout));
    // automatically release mutex when the function returns
    auto mutexRelease =
        Patterns::make_scopeExit(&RTOS::Mutex::tryRelease, transferLock);

    // Setting up device frequency
    nrfxInstance.config.frequency =
        static_cast<nrf_twim_frequency_t>(frequency);
    RETURN_ON_ERROR(
        Port::Utility::getError(nrfx_twim_init(&nrfxInstance.twimInstance,
                                               &nrfxInstance.config,
                                               &onTransferComplete,
                                               &transferComplete)));
    nrfx_twim_enable(&nrfxInstance.twimInstance);
    // make sure twimInstance is freed when the function exits
    auto twiDeinit = Patterns::make_scopeExit([this]() {
        nrfx_twim_disable(&nrfxInstance.twimInstance);
        nrfx_twim_uninit(&nrfxInstance.twimInstance);
        errata89Workaround();
        transferComplete.reset();
    });

    std::unique_ptr<uint8_t[]> setupBuffer {new uint8_t[registerAddressSize]};
    memcpy(setupBuffer.get(), registerAddress, registerAddressSize);

    // Select the starting address for reading
    RETURN_ON_ERROR(
        Port::Utility::getError(nrfx_twim_tx(&nrfxInstance.twimInstance,
                                             deviceAddress,
                                             setupBuffer.get(),
                                             registerAddressSize,
                                             false)));

    RETURN_ON_ERROR(transferComplete.await(timeoutTimer.timeLeft()));
    transferComplete.reset();

    // Reading from the selected address
    setupBuffer.reset(new uint8_t[size]);
    RETURN_ON_ERROR(
        Port::Utility::getError(nrfx_twim_rx(&nrfxInstance.twimInstance,
                                             deviceAddress,
                                             setupBuffer.get(),
                                             size)));

    // Blocking until the transfer is complete
    RETURN_ON_ERROR(transferComplete.await(timeoutTimer.timeLeft()));

    // copy out data to user field
    memcpy(data, setupBuffer.get(), size);
    return Error::None;
}

/**
 * @brief This issue causes a 450µA current increase.
 * 
 * @details When using TWIM and GPIOTE IN at the same time,
 * the TWIM will keep the base clock occupied. This will draw
 * 450µA continously. Therefor the TWIM registers for that are
 * manually switched of.
 * 
 */
void IO::I2C::Bus::errata89Workaround()
{
    if (nrfxInstance.twimInstance.drv_inst_idx == NRFX_TWIM0_INST_IDX) {
        *(volatile uint32_t*)0x40003FFC = 0;
        *(volatile uint32_t*)0x40003FFC;
        *(volatile uint32_t*)0x40003FFC = 1;
    } else if (nrfxInstance.twimInstance.drv_inst_idx == NRFX_TWIM1_INST_IDX) {
        *(volatile uint32_t*)0x40004FFC = 0;
        *(volatile uint32_t*)0x40004FFC;
        *(volatile uint32_t*)0x40004FFC = 1;
    }
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Allocates the next free I2C instance
 * 
 * @warning Only use when constructing in static context.
 * Will escalate on failure.
 * 
 * @return nrfxInstanceData& 
 */
IO::I2C::Bus::nrfxInstanceData* IO::I2C::Bus::allocInstance()
{
    for (auto& instance : nrfxInstances) {
        if (!instance.allocated) {
            instance.allocated = true;
            instance.config    = {0};
            return &instance;
        }
    }

    CHECK_ERROR(Error::OutOfResources);
    return nullptr;
}

/**
 * @brief Interrupt service routine called each time data transfer
 * is finished.
 * 
 * @param p_event Nrfx identifier and data for event
 * @param p_context Context passed by application, in this case event pointer to trigger
 */
void IO::I2C::Bus::onTransferComplete(nrfx_twim_evt_t const* p_event,
                                      void*                  p_context)
{
    // Retrieve event from context
    RTOS::Event* event = static_cast<RTOS::Event*>(p_context);
    if (!event) {
        // could not parse event, let event timeout
        return;
    }

    // Set transfer complete event flag
    bool contextSwitchNeeded = false;
    event->triggerFromISR(&contextSwitchNeeded);

    if (contextSwitchNeeded) {
        // if event unblocked the task waiting for it, do not wait for next tick to run
        RTOS::yieldToSchedulerFromISR();
    }
}

#endif
