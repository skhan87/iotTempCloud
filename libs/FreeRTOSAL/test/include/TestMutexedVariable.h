/**
 * @file TestMutexedVariable.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS mutexed variable
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTMUTEXEDVARIABLE_H__
#define __TESTMUTEXEDVARIABLE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class MutexedVariable;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_MutexedVariable.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for RTOS mutexed variable
 */
class MutexedVariable : public Test::Base
{
    // constructors
public:
    // delete default constructors
    MutexedVariable(const MutexedVariable &other) = delete;
    MutexedVariable &operator=(const MutexedVariable &other) = delete;

    /** singleton getInstance */
    static MutexedVariable &getInstance();

private:
    MutexedVariable();

protected:
    virtual void runInternal() final;

private:
    RTOS::MutexedVariable<uint64_t> mtxVar1;

    /** singleton instance */
    static MutexedVariable instance;
};
}  // namespace Test
#endif  //__TESTMUTEXEDVARIABLE_H__