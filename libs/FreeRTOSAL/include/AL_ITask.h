/**
 * @file ITask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief interface for tasks
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __ITASK_H__
#define __ITASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class ITask;
}

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>
#include "AL_RTOS.h"
#include "FreeRTOSUtility.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief abstract class for runable tasks.
 *
 */
class ITask
{
    // structs and enums
public:
    // constructors
public:
    // exposed functions
public:
    // interface
public:
    virtual void resume()                                  = 0;
    virtual void resumeFromISR(bool &contextSwitchNeeded)  = 0;
    virtual void suspend()                                 = 0;
    virtual void suspendFromISR(bool &contextSwitchNeeded) = 0;
    virtual void delay(milliseconds timeMs)                = 0;

    // private variables
private:
    // private functions
private:
    // static functions
public:
    static void yield();
    static void delayCurrentTask(milliseconds time);

protected:
};
}  // namespace RTOS
#endif  //__ITASK_H__