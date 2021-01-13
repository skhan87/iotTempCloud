/**
 * @file AL_PeriodicTask.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief periodic task implementation
 * @version 1.0
 * @date 2020-03-05
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_PeriodicTask.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct and start a periodic task
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation
 * @param context Context to execute the task on. Must have onStart() and onRun() implementation
 * @param name String identifier of the task
 * @param priority Task priority when scheduling
 * @param interval Interval in which onTime() is invoked.
 */
template<size_t StackSize, class ContextT>
RTOS::PeriodicTask<StackSize, ContextT>::PeriodicTask(ContextT&         context,
                                                      const char* const name,
                                                      uint8_t      priority,
                                                      milliseconds interval)
        : RTOS::Task<StackSize, PeriodicTask>(*this, name, priority),
          intervalTicks {Utility::millisToTicks(interval)},
          ticksTimeLast {xTaskGetTickCount()}, context {context}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Get the Interval in ms
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation
 * @return RTOS::milliseconds Interval of the periodic task in ms
 */
template<size_t StackSize, class ContextT>
RTOS::milliseconds RTOS::PeriodicTask<StackSize, ContextT>::getInterval()
{
    return Utility::ticksToMillis(intervalTicks);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief Forwards to context.onStart
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation
 */
template<size_t StackSize, class ContextT>
void RTOS::PeriodicTask<StackSize, ContextT>::onStart()
{
    context.onStart();
}

/**
 * @brief Manages the timing of the periodic task and calls onTime() in context
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation
 */
template<size_t StackSize, class ContextT>
void RTOS::PeriodicTask<StackSize, ContextT>::onRun()
{
    // execute actual function
    context.onTime();

    // FreeRTOS manages the delay
    vTaskDelayUntil(&ticksTimeLast, intervalTicks);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------