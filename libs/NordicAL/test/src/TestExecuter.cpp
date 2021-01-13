/**
 * @file TestExecuter.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief executes the testing suite
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestExecuter.h"

#include "AL_Log.h"
#include "TestBase.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::TestExecuter Test::TestExecuter::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::TestExecuter::TestExecuter() : task(*this, "TestExecuter", 1) {}

Test::TestExecuter& Test::TestExecuter::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief Task implementation
 * 
 */
void Test::TestExecuter::onStart()
{
    // do nothing
    task.delay(3000);
}

/**
 * @brief Task implementation
 * 
 */
void Test::TestExecuter::onRun()
{
    Test::Base::runAllTests();
    task.suspend();
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------