/**
 * @file FreeRTOSTask.c
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction layer for FreeRTOS task
 * @version 1.0
 * @date 2019-11-13
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Task.h"
#include <FreeRTOS.h>
#include "Future.h"

//-------------------------------- CONSTANTS ----------------------------------

//--------------------------- STRUCTS AND ENUMS -------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//---------------------------- STATIC VARIABLES -------------------------------

//----------------------------- EXPOSED FUNCTIONS -----------------------------

/**
 * @brief Construct a new Task that is running in context.
 * 
 * @warning context must not be stack allocated!
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context
 * @param context Heap or statically allocated context
 * @param name Task namme
 * @param priority Priority of the task
 */
template<std::size_t StackSize, class ContextT>
RTOS::Task<StackSize, ContextT>::Task(ContextT&         context,
                                      const char* const name,
                                      uint8_t           priority)
        : handle {xTaskCreateStatic(&Task::runRedirect,
                                    name,
                                    StackSize,
                                    &context,
                                    priority,
                                    stack,
                                    &data)}
{
    if (handle == NULL) {
        // task was not initialized
        CHECK_ERROR(Error::Unknown);
    }
}

/**
 * @brief Deletes the task before it gets destroyed.
 * 
 * @tparam StackSize 
 * @tparam ContextT 
 */
template<std::size_t StackSize, class ContextT>
RTOS::Task<StackSize, ContextT>::~Task()
{
    vTaskDelete(handle);
}

template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::resume()
{
    vTaskResume(handle);
}

template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::resumeFromISR(bool& contextSwitchNeeded)
{
    contextSwitchNeeded = xTaskResumeFromISR(handle) == pdTRUE;
}

template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::suspend()
{
    vTaskSuspend(handle);
}

template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::suspendFromISR(bool& contextSwitchNeeded)
{
    contextSwitchNeeded = false;
    suspend();
}

/**
 * @brief will delay for at least the number of given ms.
 * Ms get rounded up to ticks, then that might get delayed
 * further by higher priority tasks.
 *
 * @param time Time to delay
 */
template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::delay(milliseconds time)
{
    ITask::delayCurrentTask(time);
}

/**
 * @brief Redirects the task call to its context.
 * 
 * @details Takes the contextPtr and casts it to ContextT*.
 * Then invokes onStart() and onRun() on that instance.
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context
 * @param contextPtr Context pointer passed to FreeRTOS task create.
 */
template<std::size_t StackSize, class ContextT>
void RTOS::Task<StackSize, ContextT>::runRedirect(void* contextPtr)
{
    auto castContextPtr = static_cast<ContextT*>(contextPtr);
    if (castContextPtr == nullptr) {
        // must not happen
        CHECK_ERROR(Error::Internal);
    }

    castContextPtr->onStart();

    while (1) {
        castContextPtr->onRun();
        // allow scheduler to check for other tasks
        taskYIELD();
    }
}

/**
 * @brief Get the Name of the task
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context
 * @return const char* const Name of the task
 */
template<std::size_t StackSize, class ContextT>
const char* const RTOS::Task<StackSize, ContextT>::getName()
{
    return pcTaskGetName(handle);
}

//---------------------------- STATIC FUNCTIONS -------------------------------