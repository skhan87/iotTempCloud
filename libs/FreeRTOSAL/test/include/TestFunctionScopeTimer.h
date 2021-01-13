/**
 * @file TestFunctionScopeTimer.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for the FunctionScopeTimer class
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTFUNCTIONSCOPETIMER_H__
#define __TESTFUNCTIONSCOPETIMER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class FunctionScopeTimer;
}

//--------------------------------- INCLUDES ----------------------------------

#include "TestBase.h"
#include "FunctionScopeTimer.h"
#include "AL_RTOS.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Test for the FunctionScopeTimer class
 */
class FunctionScopeTimer : public Test::Base {
    // constructors
public:
    // delete default constructors
    FunctionScopeTimer(const FunctionScopeTimer& other) = delete;
    FunctionScopeTimer& operator=(const FunctionScopeTimer& other) = delete;

    /**
    * @brief get singleton instance
    */
    static FunctionScopeTimer& getInstance();

private:
    FunctionScopeTimer();

    virtual void runInternal() final;

    /** singleton instance */
    static FunctionScopeTimer instance;
};
}  // namespace Test
#endif  //__TESTFUNCTIONSCOPETIMER_H__