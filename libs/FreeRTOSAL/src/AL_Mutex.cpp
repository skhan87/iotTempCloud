/**
 * @file AL_Mutex.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for mutex
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Mutex.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

RTOS::Mutex::Mutex() : handle(xSemaphoreCreateMutexStatic(&semaphoreData))
{
}

RTOS::Mutex::~Mutex()
{
    vSemaphoreDelete(handle);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

Error::Code RTOS::Mutex::tryObtain(RTOS::milliseconds timeout)
{
    if (xSemaphoreTake(handle, Utility::millisToTicks(timeout)) == pdTRUE) {
        return Error::None;
    } else {
        return Error::Timeout;
    }
}

Error::Code RTOS::Mutex::tryObtainFromISR(bool *contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken;
    auto       success =
        xSemaphoreTakeFromISR(handle, &pxHigherPriorityTaskWoken);

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = (pxHigherPriorityTaskWoken == pdTRUE);
    }

    if (success == pdTRUE) {
        return Error::None;
    } else {
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Mutex::tryRelease()
{
    if (xSemaphoreGive(handle) == pdTRUE) {
        return Error::None;
    } else {
        return Error::InvalidUse;
    }
}

Error::Code RTOS::Mutex::tryReleaseFromISR(bool *contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken;
    auto       success =
        xSemaphoreGiveFromISR(handle, &pxHigherPriorityTaskWoken);
    
    if (contextSwitchNeeded) {
        *contextSwitchNeeded = (pxHigherPriorityTaskWoken == pdTRUE);
    }

    if (success == pdTRUE) {
        return Error::None;
    } else {
        return Error::Unknown;
    }
}

bool RTOS::Mutex::isLocked()
{
    return uxSemaphoreGetCount(handle) == 0;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------