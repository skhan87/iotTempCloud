/**
 * @file TestExecuter.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief executes the test module
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTEXECUTER_H__
#define __TESTEXECUTER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class TestExecuter;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_RTOS.h"
#include "AL_Task.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief executes the test module.
 * 
 * @details Implemented as eager loading singleton.
 */
class TestExecuter {
    static constexpr size_t kStackSize =
        1024; /**< Stack size of the task executing the tests */

    friend RTOS::Task<kStackSize, TestExecuter>;

public:
    // delete default constructors
    TestExecuter(const TestExecuter& other) = delete;
    TestExecuter& operator=(const TestExecuter& other) = delete;

    /** singleton getInstance() */
    static TestExecuter& getInstance();

private:
    // task interface
    void onStart();
    void onRun();

    TestExecuter();

    RTOS::Task<kStackSize, TestExecuter>
        task; /**< Task that executes tests in context of this class */

    /** singleton instance */
    static TestExecuter instance;

    static constexpr uint8_t kTaskPriority =
        1; /**< Not too high, logger e.g. shall still run properly */
};
}  // namespace Test
#endif  //__TESTEXECUTER_H__