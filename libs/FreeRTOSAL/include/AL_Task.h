/**
 * @file FreeRTOSTask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for FreeRTOS Tasks
 * @version 1.0
 * @date 2019-12-10
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

#ifndef __FREERTOSTASK_H__
#define __FREERTOSTASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

#include <stddef.h>

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include <task.h>
#include "AL_ITask.h"
#include "FreeRTOSUtility.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Class for tasks.
 * 
 * @warning Do not use with stack allocation!
 * 
 * @tparam StackSize Size of the task stack
 * @tparam ContextT Type of the context which holds the task Implementation.
 * 
 * @details To use the Task class, create an instance and pass
 * object that has
 * void ContextT::onRun();
 * void ContextT::onStart();
 * implemented.
 * 
 * @example Here a simple usage example
 * ```cpp
 * class MyContext {
 *     // befriend task to allow access to private members
 *     friend RTOS::Task<256, MyContext>;
 * private:
 *     RTOS::Task<256, MyContext> rtosTask;
 *  
 *     void onStart() {
 *         LOG_I("Task started");
 *     }
 * 
 *     void onRun() {
 *         LOG_I("Task running");
 *         delay(1000);
 *     }
 * 
 * public:
 *     MyContext() : rtosTask(*this, "task1", 1) {}
 * };
 * 
 * static Mycontext context1{};
 * ```
 */
template<size_t StackSize, class ContextT>
class Task : public RTOS::ITask {
public:
    /** maximum priority a task can have */
    static constexpr uint8_t MaxPriority = configMAX_PRIORITIES - 1;

    // delete copy constructors
    Task()                  = delete;
    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;

    Task(ContextT& context, const char* const name, uint8_t priority);
    ~Task();

    const char* const getName();

    virtual void resume() final;
    virtual void resumeFromISR(bool& contextSwitchNeeded) final;
    virtual void suspend() final;
    virtual void suspendFromISR(bool& contextSwitchNeeded) final;
    virtual void delay(milliseconds time) final;

private:
    TaskHandle_t handle; /**< Handle of the task used by FreeRTOS */
    StaticTask_t data; /**< Data needed by FreeRTOS to manage this task */
    StackType_t  stack[StackSize]; /**< Static memory for task stack */

    static void runRedirect(void* contextPtr);
};
}  // namespace RTOS

#include "../src/AL_Task.cpp"
#endif  //__FREERTOSTASK_H__
