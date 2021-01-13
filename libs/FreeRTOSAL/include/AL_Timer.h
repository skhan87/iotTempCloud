/**
 * @file AL_Timer.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for timers
 * @version 1.0
 * @date 2020-03-05
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_TIMER_H__
#define __AL_TIMER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class Timer;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_RTOS.h"
#include "FreeRTOSUtility.h"

#include <Error.h>
#include <FreeRTOS.h>
#include <timers.h>

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief timer that calls ISR when expired.
 *
 */
class Timer {
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    Timer()                   = delete;
    Timer(const Timer& other) = delete;
    Timer& operator=(const Timer& other) = delete;

    /**
     * @brief Construct a new Timer
     *
     * @param timerMs
     */
    Timer(const char* const name, milliseconds timeMs, bool restarting);
    /**
     * @brief Destroy the Timer
     *
     */
    ~Timer();

    // exposed functions
public:
    /**
     * @brief start the timer.
     * Timer should be in idle when this is used.
     *
     * @param timeoutMs
     * @return Error::Code 
     */
    Error::Code start(milliseconds timeoutMs = Infinity);
    /**
     * @brief start from interrupt service routine
     *
     * @param contextSwitchNeeded calling ISR needs to yield to scheduler
     * @return Error::Code 
     */
    Error::Code startFromISR(bool* contextSwitchNeeded = nullptr);
    /**
     * @brief stops the timer
     *
     * @param timeoutMs time to wait for timer to react
     * @return Error::Code 
     */
    Error::Code stop(milliseconds timeoutMs = Infinity);
    /**
     * @brief stops the timer from iterrupt service routine
     *
     * @param contextSwitchNeeded calling ISR need to yield to scheduler
     * @return Error::Code 
     */
    Error::Code stopFromISR(bool* contextSwitchNeeded = nullptr);
    /**
     * @brief If stopped, timer will start, if running, time will be reset
     *
     * @param timeoutMs time to wait for timer to reset
     * @return Error::Code 
     */
    Error::Code reset(milliseconds timeoutMs = Infinity);
    /**
     * @brief If stopped, timer will start, if running, time will be reset.
     * Call from interrupt service routine
     *
     * @param contextSwitchNeeded calling ISR need to yield to scheduler
     * @return Error::Code 
     */
    Error::Code resetFromISR(bool* contextSwitchNeeded = nullptr);
    /**
     * @brief Get the Name of this timer
     *
     * @return const char*
     */
    const char* getName();
    /**
     * @brief Set the Total Time Ms
     *
     * @param timeMs new period
     * @param timeoutMs time to wait
     * @return Error::Code 
     */
    Error::Code setTotalTimeMs(milliseconds timeMs,
                               milliseconds timeoutMs = Infinity);
    /**
     * @brief Set the Total Time from ISR
     *
     * @param timeMs new period
     * @param contextSwitchNeeded whether the calling ISR needs to yield to
     * scheduler
     * @return Error::Code 
     */
    Error::Code setTotalTimeMsFromISR(milliseconds timeMs,
                                      bool* contextSwitchNeeded = nullptr);

    /**
     * @brief Get the total time in ms
     *
     * @return milliseconds
     */
    milliseconds getTotalTimeMs();
    /**
     * @brief Get the Remaining Time in Ms
     *
     * @return milliseconds
     */
    milliseconds getRemainingTimeMs();
    /**
     * @brief if the timer is currently active
     *
     * @return true
     * @return false
     */
    bool isActive();

    // interface implementation
public:
    // private variables
private:
    /**
     * @brief FreeRTOS timer handle
     *
     */
    TimerHandle_t handle;
    /**
     * @brief FreeRTOS timer data
     *
     */
    StaticTimer_t data;

    // private functions
private:
    // static functions
private:
    static void callback(TimerHandle_t xTimer);

protected:
    /**
     * @brief needs to be implemented to determine the behaviour of a timer
     *
     */
    virtual void onTimer();
};
}  // namespace RTOS
#endif  //__AL_TIMER_H__