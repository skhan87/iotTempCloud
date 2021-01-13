/**
 * @file AL_RTOS.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction of generel RTOS functions
 * @version 1.0
 * @date 2020-03-09
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_RTOS.h"
#include <Error.h>
#include "PatternsPort.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief initializes Scheduler and then blocks for it.
 * Function never returns.
 * Call as last function in main.
 *
 */
void RTOS::init()
{
    vTaskStartScheduler();
}

/**
 * @brief call to enable scheduler to switch from ISR.
 *
 * 
 * @details If ISR functions report they need a context switch, use this.
 * Corresponding yield for tasks can only be called from within task context.
 *
 */
void RTOS::yieldToSchedulerFromISR()
{
    portYIELD_FROM_ISR(pdTRUE);
}

/**
 * @brief Get the time since scheduler has been started in ms
 *
 * @return milliseconds
 */
RTOS::milliseconds RTOS::getTime()
{
    TimeOut_t time;
    vTaskSetTimeOutState(&time);
    return time.xOverflowCount * portMAX_DELAY + time.xTimeOnEntering;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

static StaticTask_t timerTask;
static StaticTask_t idleTask;
static StackType_t  timerStack[configTIMER_TASK_STACK_DEPTH];
static StackType_t  idleStack[configMINIMAL_STACK_SIZE];

/**
 * @brief Hook for getting timer task memory
 * 
 * @param ppxTimerTaskTCBBuffer Static memory ptr for task variables
 * @param ppxTimerTaskStackBuffer Static memory for build task stack to
 * @param pulTimerTaskStackSize Size of the stack memory
 */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                    StackType_t**  ppxTimerTaskStackBuffer,
                                    uint32_t*      pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer   = &timerTask;
    *ppxTimerTaskStackBuffer = timerStack;
    *pulTimerTaskStackSize   = sizeof(timerStack) / sizeof(timerStack[0]);
}

/**
 * @brief Hook for getting idle task memory
 * 
 * @param ppxIdleTaskTCBBuffer Static memory ptr for task variables
 * @param ppxIdleTaskStackBuffer Static memory for build task stack to
 * @param pulIdleTaskStackSize Size of the stack memory
 */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                   StackType_t**  ppxIdleTaskStackBuffer,
                                   uint32_t*      pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &idleTask;
    *ppxIdleTaskStackBuffer = idleStack;
    *pulIdleTaskStackSize   = sizeof(idleStack) / sizeof(idleStack[0]);
}

/**
 * @brief Hook for detecting stack overflows
 * 
 * @warning If this gets called, increase the stack size
 * of the corresponding task.
 * 
 * @param xTask RTOS handle of the task thats stack overflew
 * @param pcTaskName 
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName)
{
    // for detection during debugging
    Port::logInfo("Task violated stack frame: \"");
    Port::logInfo(reinterpret_cast<const char* const>(pcTaskName));
    Port::logInfo("\"\n");
    CHECK_ERROR(Error::StackCorrupt);
}

/**
 * @brief Hook for detecting heap is full
 *
 */
void vApplicationMallocFailedHook(void)
{
    // for detection during debugging
    CHECK_ERROR(Error::OutOfResources);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------