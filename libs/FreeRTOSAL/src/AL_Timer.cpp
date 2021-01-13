/**
 * @file AL_Timer.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for timers
 * @version 1.0
 * @date 2020-03-05
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Timer.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

RTOS::Timer::Timer(const char* const name, milliseconds time, bool restarting)
    : handle(xTimerCreateStatic(name,
                                Utility::millisToTicks(time),
                                restarting ? pdTRUE : pdFALSE,
                                this,
                                callback,
                                &data))
{}

RTOS::Timer::~Timer()
{
    xTimerDelete(handle, portMAX_DELAY);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

Error::Code RTOS::Timer::start(milliseconds timeout)
{
    if (xTimerStart(handle, Utility::millisToTicks(timeout)) == pdPASS) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::startFromISR(bool* contextSwitchNeeded)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    auto       success =
        xTimerStartFromISR(handle, &higherPriorityTaskWoken) == pdPASS;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = higherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::stop(milliseconds timeout)
{
    if (xTimerStop(handle, Utility::millisToTicks(timeout)) == pdPASS) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::stopFromISR(bool* contextSwitchNeeded)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    auto       success =
        xTimerStopFromISR(handle, &higherPriorityTaskWoken) == pdPASS;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = higherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::reset(milliseconds timeout)
{
    if (xTimerReset(handle, Utility::millisToTicks(timeout)) == pdPASS) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::resetFromISR(bool* contextSwitchNeeded)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    auto       success =
        xTimerResetFromISR(handle, &higherPriorityTaskWoken) == pdPASS;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = higherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

const char* RTOS::Timer::getName()
{
    return pcTimerGetName(handle);
}

Error::Code RTOS::Timer::setTotalTimeMs(milliseconds time, milliseconds timeout)
{
    if (xTimerChangePeriod(handle,
                           Utility::millisToTicks(time),
                           Utility::millisToTicks(timeout)) == pdPASS) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

Error::Code RTOS::Timer::setTotalTimeMsFromISR(milliseconds time,
                                               bool*        contextSwitchNeeded)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    auto       success =
        xTimerChangePeriodFromISR(handle,
                                  Utility::millisToTicks(time),
                                  &higherPriorityTaskWoken) == pdPASS;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = higherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        return Error::None;
    } else {
        // Timer queue full
        return Error::OutOfResources;
    }
}

RTOS::milliseconds RTOS::Timer::getTotalTimeMs()
{
    return Utility::ticksToMillis(xTimerGetPeriod(handle));
}

RTOS::milliseconds RTOS::Timer::getRemainingTimeMs()
{
    return Utility::ticksToMillis(xTimerGetExpiryTime(handle) -
                                  xTaskGetTickCount());
}

bool RTOS::Timer::isActive()
{
    return xTimerIsTimerActive(handle) != pdFALSE;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void RTOS::Timer::onTimer()
{
    // don't do anything
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

void RTOS::Timer::callback(TimerHandle_t xTimer)
{
    reinterpret_cast<Timer*>(pvTimerGetTimerID(xTimer))->onTimer();
}