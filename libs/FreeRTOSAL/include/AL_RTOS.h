/**
 * @file AL_RTOS.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for general RTOS functions
 * @version 1.0
 * @date 2020-03-09
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_RTOS_H__
#define __AL_RTOS_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include <stdbool.h>
#include <task.h>
#include <cstdint>
#include "FreeRTOSConfig.h"
#include <limits>

extern "C" void
    vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                   StackType_t**  ppxTimerTaskStackBuffer,
                                   uint32_t*      pulTimerTaskStackSize);

extern "C" void
    vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                  StackType_t**  ppxIdleTaskStackBuffer,
                                  uint32_t*      pulIdleTaskStackSize);

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              signed char* pcTaskName);

extern "C" void vApplicationMallocFailedHook(void);

namespace RTOS
{
//---------------------------- ENUMS AND STRUCTS ------------------------------

/** @brief unit used for all timings milliseconds */
using milliseconds = int64_t;

//-------------------------------- CONSTANTS ----------------------------------

/** use, if function should block forever */
constexpr milliseconds Infinity = std::numeric_limits<int64_t>::max();

//----------------------------- PUBLIC FUNCTIONS ------------------------------

void         init();
void         yieldToSchedulerFromISR();
milliseconds getTime();

}  // namespace RTOS
#endif  //__AL_RTOS_H__