/**
 * @file TestSemaphore.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS semaphore implementation
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTSEMAPHORE_H__
#define __TESTSEMAPHORE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Semaphore;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_CountingSemaphore.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for RTOS semaphore implementation
 */
class Semaphore : public Test::Base
{
    // constructors
public:
    // delete default constructors
    Semaphore(const Semaphore &other) = delete;
    Semaphore &operator=(const Semaphore &other) = delete;

    /** singleton getInstance() */
    static Semaphore& getInstance();

private:
    Semaphore();

protected:
    virtual void runInternal() final;

private:
    static constexpr size_t Sem1Size = 5;
    RTOS::CountingSemaphore sem1;

    /** singleton instance */
    static Semaphore instance;
};
}  // namespace Test
#endif  //__TESTSEMAPHORE_H__