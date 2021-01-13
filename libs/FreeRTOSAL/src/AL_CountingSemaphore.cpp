/**
 * @file AL_CountingSemaphore.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for counting semaphore
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_CountingSemaphore.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new Counting Semaphore object.
 *
 */
RTOS::CountingSemaphore::CountingSemaphore(uint32_t maxCount,
                                           uint32_t startingCount)
        : handle(xSemaphoreCreateCountingStatic(maxCount,
                                                startingCount,
                                                &semaphoreData))
{}

/**
 * @brief Destroy the Counting Semaphore object
 *
 */
RTOS::CountingSemaphore::~CountingSemaphore()
{
    vSemaphoreDelete(handle);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Decrease semaphore counter by 1
 *
 * @param timeoutMs time to wait
 * @return Error::Timeout if count == 0 for the whole timeoutMs
 */
Error::Code RTOS::CountingSemaphore::take(milliseconds timeout)
{
    if (xSemaphoreTake(handle, Utility::millisToTicks(timeout)) == pdTRUE) {
        return Error::None;
    } else {
        return Error::Timeout;
    }
}

/**
 * @brief Tries to decrease Semaphore counter from ISR
 *
 * @param contextSwitchNeeded if the calling ISR should cause a context
 * switch
 * @return Error::MinReached if count == 0
 */
Error::Code RTOS::CountingSemaphore::takeFromISR(bool* contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken;
    auto result = xSemaphoreTakeFromISR(handle, &pxHigherPriorityTaskWoken);

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = (pxHigherPriorityTaskWoken == pdTRUE);
    }

    if (result == pdTRUE) {
        return Error::None;
    } else {
        return Error::MinReached;
    }
}

/**
 * @brief Increase semaphore counter by 1
 *
 * @return Error::MaxReached if count is already countMax
 */
Error::Code RTOS::CountingSemaphore::give()
{
    if (xSemaphoreGive(handle) == pdTRUE) {
        return Error::None;
    } else {
        return Error::MaxReached;
    }
}

/**
 * @brief Increase the semaphore counter from ISR
 *
 * @param contextSwitchNeeded if calling ISR has to switch context
 * @return Error::MaxReached if count is already countMax
 */
Error::Code RTOS::CountingSemaphore::giveFromISR(bool* contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken;
    auto success = xSemaphoreGiveFromISR(handle, &pxHigherPriorityTaskWoken);

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = (pxHigherPriorityTaskWoken == pdTRUE);
    }

    if (success == pdTRUE) {
        return Error::None;
    } else {
        return Error::MaxReached;
    }
}

/**
 * @brief Return the current count of this semaphore
 *
 * @return uint32_t current semaphore counter value
 */
uint32_t RTOS::CountingSemaphore::getCount()
{
    return uxSemaphoreGetCount(handle);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------