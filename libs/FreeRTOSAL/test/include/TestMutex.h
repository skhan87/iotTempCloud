/**
 * @file TestMutex.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief testing RTOS mutex implementation
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTMUTEX_H__
#define __TESTMUTEX_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Mutex;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Mutex.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief testing RTOS mutex implementation
 */
class Mutex : public Test::Base
{
    // constructors
public:
    // delete default constructors
    Mutex(const Mutex &other) = delete;
    Mutex &operator=(const Mutex &other) = delete;

    /**
     * @brief get singleton instance
     *
     */
    static Mutex &getInstance();

private:
    Mutex();

protected:
    virtual void runInternal() final;

private:
    RTOS::Mutex mutex1;

    /** singleton instance */
    static Mutex instance;
};
}  // namespace Test
#endif  //__TESTMUTEX_H__