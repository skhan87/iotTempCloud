/**
 * @file TestMutexedVariable.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS MUtexedVariable
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestMutexedVariable.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::MutexedVariable Test::MutexedVariable::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::MutexedVariable::MutexedVariable() :
    Test::Base("RTOS", "MutexedVariable"), mtxVar1(1)
{
}

Test::MutexedVariable &Test::MutexedVariable::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::MutexedVariable::runInternal()
{
    uint64_t v1 = 0;
    assert(mtxVar1.tryGet(v1, 10) == Error::None, "failed to get");
    assert(v1 == 1, "Mutexed variable has wrong start value");
    v1 = 2;
    assert(mtxVar1.trySet(v1, 10) == Error::None, "failed to set");
    assert(mtxVar1.tryGet(v1, 10) == Error::None, "failed to get");
    assert(v1 == 2, "wrong value after setting to 2");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------