/**
 * @file FunctionScopeTimer.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Timer to keep track of async functions runtime.
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "FunctionScopeTimer.h"
#include <limits>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Instantiate FunctionScopeTime.
 * 
 * @warning Only use to instantiate variables on stack.
 * 
 * @details Use this to check your task function durations and runtimes.
 * 
 * @param timeout 
 */
RTOS::FunctionScopeTimer::FunctionScopeTimer(RTOS::milliseconds timeout)
        : outTime((timeout == RTOS::Infinity) ? RTOS::Infinity
                                              : (RTOS::getTime() + timeout))
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Check whether the time for the given function timer ran out.
 *
 * @return true Function timeout is overstepped
 * @return false Function timeout not yet overstepped
 */
bool RTOS::FunctionScopeTimer::hasRunOut()
{
    // always hadle this case explicitly
    if (outTime == RTOS::Infinity) {
        return false;
    }

    return RTOS::getTime() > outTime;
}

/**
 * @brief Check whether the timeout will be overstepped after given time.
 * 
 * @param time Time to add to current time in ms
 * @return true Function timeout will be overstepped in time
 * @return false Function timeout will not be overstepped in time
 */
bool RTOS::FunctionScopeTimer::willBeRunOutIn(RTOS::milliseconds time)
{
    // always hadle this case explicitly
    if (outTime == RTOS::Infinity) {
        return false;
    }

    auto currentTime = RTOS::getTime();

    // check overflow, which is really really unlikely to happen with int64_t
    if (((currentTime / 2) + (time / 2)) >
        (std::numeric_limits<int64_t>::max() / 2)) {
        return true;
    }

    return (currentTime + time) > outTime;
}

/**
 * @brief Returns the timee left on this function timer.
 * 
 * @return RTOS::milliseconds Time that's left on the timer. 0 if timer has run out.
 */
RTOS::milliseconds RTOS::FunctionScopeTimer::timeLeft()
{
    // always hadle this case explicitly
    if (outTime == RTOS::Infinity) {
        return RTOS::Infinity;
    }

    // always hadle this case explicitly
    auto currentTime = RTOS::getTime();
    if (currentTime > outTime) {
        return 0;
    }

    return outTime - currentTime;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------