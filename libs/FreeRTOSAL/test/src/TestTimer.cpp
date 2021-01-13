/**
 * @file TestTimer.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for timers
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestTimer.h"
#include "AL_RTOS.h"
#include "TestEvent.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Timer Test::Timer::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Timer::Timer() :
    Test::Base("RTOS", "Timer"), RTOS::Timer("TestTimer", 100, false), grp(),
    evtOnTime(grp)
{
}

Test::Timer &Test::Timer::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Timer::runInternal()
{
    assert(start(10) == Error::None, "Could not start timer");
    assert(evtOnTime.await(60) != Error::None,
           "evt triggered too early");
    assert(evtOnTime.await(100) == Error::None,
           "evt did not trigger on time");
}

const std::list<Test::Base *> Test::Timer::getPrerequisits()
{
    return std::list<Test::Base *>({&Test::Event::getInstance()});
}

void Test::Timer::onTimer()
{
    // not an ISR!!
    evtOnTime.trigger();
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------