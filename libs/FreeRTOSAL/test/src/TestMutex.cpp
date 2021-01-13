/**
 * @file TestMutex.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief testing RTOS Mutex
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestMutex.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Mutex Test::Mutex::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Mutex::Mutex() : Base("RTOS", "Mutex"), mutex1()
{
}

Test::Mutex &Test::Mutex::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Mutex::runInternal()
{
    assert(!mutex1.isLocked(), "mutex not unlocked after constructing");
    assert(mutex1.tryObtain(1) == Error::None, "failed to obtain mutex");
    assert(mutex1.isLocked(), "mutex isLocked() returns wrong state");
    assert(mutex1.tryRelease() == Error::None, "failed to release mutex");
    assert(!mutex1.isLocked(), "mutex isLocked() return wrong state");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
