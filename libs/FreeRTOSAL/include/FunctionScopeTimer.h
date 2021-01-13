/**
 * @file FunctionScopeTimer.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Timer to keep track of async functions runtime.
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __FUNCTIONSCOPETIMER_H__
#define __FUNCTIONSCOPETIMER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class FunctionScopeTimer;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_RTOS.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Timer to keep track of async functions runtime.
 * 
 * @warning Only use with stack allocation (inside functions).
 * Behaviour is undefined otherwise.
 * 
 * @warning No interrupts or callbacks can be hooked. This is NOT a
 * RTOS Timer, but a helper object to keep track of function runtime
 * in task context.
 * 
 * @details Gets RTOS time when created and exposes functions to check
 * against timeout in execution time.
 * 
 * @example To use the Function scope timer, just instantiate
 * in function context and use it's functions for determining
 * whether it ran out:
 * 
 * void myFunction(RTOS::milliseconds timeout)
 * {
 *     FunctionScopeTimer fctTimer{timeout};
 * 
 *     // use case #1
 *     doSomeBlockingStuff(); // this might take time
 *     if (fctTimer.hasRunOut()) {
 *         return Error::Timeout;
 *     }
 * 
 *     # use case #2
 *     otherFctWithTimeout(fctTimer.timeLeft()); // the other function can now use the leftover time
 *     otherFctWithTimeout(fctTime);             // you might as well pass the function timer
 * 
 *     # use case #3
 *     // if you know the others functions blocking time, you can check
 *     // the timeout before you enter.
 *     RTOS::milliseconds delayTime = getMyTime();
 *     if (fctTimer.willBeRunOutIn(delayTime)) {
 *         return Error::Timeout;
 *     }
 *     delay(delayTime);
 * }
 * 
 * 
 * 
 */
class FunctionScopeTimer {
    // delete default constructors
    FunctionScopeTimer()                                = delete;
    FunctionScopeTimer(const FunctionScopeTimer& other) = delete;
    FunctionScopeTimer& operator=(const FunctionScopeTimer& other) = delete;

public:
    FunctionScopeTimer(RTOS::milliseconds timeout);
    bool               hasRunOut();
    bool               willBeRunOutIn(RTOS::milliseconds time);
    RTOS::milliseconds timeLeft();

private:
    RTOS::milliseconds
        outTime; /**< timestamp when timer will run out. Infinity when Infinity timeout was chosen */
};
}  // namespace RTOS
#endif  //__FUNCTIONSCOPETIMER_H__