/**
 * @file TestFunctionScopeTimer.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for FunctionScopeTimer
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestFunctionScopeTimer.h"
#include "AL_ITask.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::FunctionScopeTimer Test::FunctionScopeTimer::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::FunctionScopeTimer::FunctionScopeTimer()
        : Base("RTOS", "FunctionScopeTimer")
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::FunctionScopeTimer::runInternal()
{
    // test simple functionality
    RTOS::FunctionScopeTimer fctTimer1 {2};
    assert(fctTimer1.hasRunOut() == false,
           "function scope timer spawned as ran out");
    assert(fctTimer1.willBeRunOutIn(0) == false, "willBeRunOutIn failed");
    assert(fctTimer1.timeLeft() > 0, "timeLeft() failed");
    RTOS::ITask::delayCurrentTask(4);
    assert(fctTimer1.hasRunOut() == true, "timerout did not run out");
    assert(fctTimer1.willBeRunOutIn(0) == true, "willBeRunOutIn failed");
    assert(fctTimer1.timeLeft() == 0, "timeLeft() failed");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

Test::FunctionScopeTimer& Test::FunctionScopeTimer::getInstance()
{
    return instance;
}