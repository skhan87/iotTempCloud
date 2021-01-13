/**
 * @file AL_PeriodicTask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief extension of task to allow excecution off periodic tasks
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_PERIODICTASK_H__
#define __AL_PERIODICTASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <cstddef>
#include "AL_Task.h"
#include "FreeRTOSUtility.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Class for implementing Periodic Tasks
 * 
 * @warning Do not instantiate on stack!
 * 
 * @details Instantiate with context object that has following functions:
 * void onStart();
 * void onTime();
 * PeriodicTask will then call that function when appropiate.
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation.
 * 
 * @example Simple usage example:
 * ```cpp
 * class MyContext {
 *     // befriend task to allow access to private members
 *     friend RTOS::PeriodicTask<256, MyContext>;
 * private:
 *     RTOS::PeriodicTask<256, MyContext> rtosTask;
 *  
 *     void onStart() {
 *         LOG_I("Task started");
 *     }
 * 
 *     void onTime() {
 *         LOG_I("Task running");
 *     }
 * 
 *     void onTimeOverflow(RTOS::milliseconds timeMissed) {
 *         LOG_E("Task overflown by %d milliseconds.", timeMissed);
 *     }
 * 
 * public:
 *     MyContext() : rtosTask(*this, "task1", 1, 1000) {}
 * };
 * 
 * static MyContext context1{};
 * ```
 */
template<size_t StackSize, class ContextT>
class PeriodicTask : public Task<StackSize, PeriodicTask<StackSize, ContextT>> {
public:
    // delete default constructors
    PeriodicTask()                          = delete;
    PeriodicTask(const PeriodicTask& other) = delete;
    PeriodicTask& operator=(const PeriodicTask& other) = delete;

    PeriodicTask(ContextT&         context,
                 const char* const name,
                 uint8_t           priority,
                 milliseconds      intervalMs);

public:
    milliseconds getInterval();

private:
    TickType_t
               intervalTicks; /**< Interval in which onTime() is executed in ticks */
    TickType_t ticksTimeLast; /**< Timestamp of last activation of the task */
    ContextT&  context; /**< Context in which the task is executedv */

    friend Task<StackSize, PeriodicTask>;
    void onStart();
    void onRun();
};
}  // namespace RTOS

#include "../src/AL_PeriodicTask.cpp"
#endif  //__AL_PERIODICTASK_H__