/**
 * @file ITask.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstract class for tasks
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include "AL_ITask.h"
#include "FreeRTOSUtility.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief yield to scheduler
 *
 */
void RTOS::ITask::yield()
{
    taskYIELD();
}

/**
 * @brief Delay the current task for given time.
 * 
 * @param time Time in milliseconds.
 */
void RTOS::ITask::delayCurrentTask(milliseconds time)
{
    vTaskDelay(Utility::millisToTicks(time));
}