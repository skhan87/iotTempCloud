/**
 * @file TestSemaphore.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS counting semaphore
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestSemaphore.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Semaphore Test::Semaphore::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Semaphore::Semaphore() :
    Test::Base("RTOS", "CountingSemaphore"), sem1(Sem1Size, Sem1Size)
{
}

Test::Semaphore &Test::Semaphore::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Semaphore::runInternal()
{
    auto cnt = sem1.getCount();
    assert(cnt == Sem1Size, "Not initialized to right value");
    for(size_t i = 0; i < Sem1Size; i++)
    {
        assert(sem1.take(10) == Error::None, "failed to take semaphore");
    }
    cnt = sem1.getCount();
    assert(cnt == 0, "semaphore count wrong (should be 0)");
    assert(sem1.take(10) == Error::Timeout, "Able to take more often than semaphore max count");
    for(size_t i = 0; i < Sem1Size; i++)
    {
        assert(sem1.give() == Error::None, "failed to give semaphore");
    }
    cnt = sem1.getCount();
    assert(cnt == Sem1Size, "semaphore count wrong");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------