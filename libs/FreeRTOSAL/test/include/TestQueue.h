/**
 * @file TestQueue.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief testig RTOS queue
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTQUEUE_H__
#define __TESTQUEUE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Queue;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Queue.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief testig RTOS queue
 */
class Queue : public Test::Base
{
    // constructors
public:
    // delete default constructors
    Queue(const Queue &other) = delete;
    Queue &operator=(const Queue &other) = delete;

    /**
     * @brief get singleton instance
     */
    static Queue &getInstance();

private:
    Queue();

    virtual void runInternal() final;

    /** queue used for testing */
    RTOS::Queue<uint64_t, 8> queue1;
    static const char *const name;

    /** singleton instance */
    static Queue instance;
};
}  // namespace Test
#endif  //__TESTQUEUE_H__