/**
 * @file AL_Utility.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief functions only for internal use of the library
 * @version 1.0
 * @date 2020-04-03
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_UTILITY_H__
#define __AL_UTILITY_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <cstdint>
#include "AL_RTOS.h"
#include "FreeRTOS.h"
#include "Error.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Encapsulated functions and definitions for FreeRTOS use.
 * 
 * @warning This functions must not be exposed to other libraries,
 * nor are classes from other modules allowed to be befriended here.
 * 
 * @details The utility class contains definitions, that need to
 * be used from several places within the library, but shall not
 * be exposed to the external user. Therefor all implementation is
 * private and classes from within this lib are befriended if
 * they need implementation from here.
 * 
 */
class Utility {
    friend class CountingSemaphore;
    friend class Mutex;
    friend class EventGroup;
    template<size_t StackSize, class ContextT>
    friend class PeriodicTask;
    template<class T, size_t queueLength>
    friend class Queue;
    friend class ITask;
    template<size_t StackSize, class ContextT>
    friend class Task;
    friend class Timer;

public:
    // delete all constructors
    Utility()                     = delete;
    Utility(const Utility& other) = delete;
    Utility& operator=(const Utility& other) = delete;

private:
    /** maximum possible wait time for OS functions */
    static constexpr milliseconds MaxWaitTime =
        portMAX_DELAY * portTICK_PERIOD_MS;

    /**
     * @brief converts milliseconds to ticks.
     * Might assert when values are too large.
     *
     * @param ms
     * @return constexpr TickType_t
     */
    static constexpr TickType_t millisToTicks(milliseconds ms)
    {
        if (ms == Infinity) {
            // is infinity in FreeRTOS
            return portMAX_DELAY;
        }

        if (ms < 0) {
            // must not be smaller 0
            CHECK_ERROR(Error::InvalidParameter);
        }
        TickType_t ticks = ms / portTICK_PERIOD_MS;
        if (ms % portTICK_PERIOD_MS > 0) {
            ++ticks;
        }
        if (ticks >= portMAX_DELAY) {
            // too large
            CHECK_ERROR(Error::InvalidParameter);
        }
        return ticks;
    }

    /**
     * @brief converts ticks to milliseconds.
     * No asserts cause here.
     *
     * @param ticks
     * @return constexpr milliseconds
     */
    static constexpr milliseconds ticksToMillis(TickType_t ticks)
    {
        if (ticks == portMAX_DELAY) {
            // is Infinity in FreeRTOS
            return Infinity;
        }

        return ticks * portTICK_PERIOD_MS;
    }
};
}  // namespace RTOS
#endif  //__AL_UTILITY_H__