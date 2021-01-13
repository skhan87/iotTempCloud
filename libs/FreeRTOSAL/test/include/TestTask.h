/**
 * @file TestTask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS tasks
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTTASK_H__
#define __TESTTASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Event.h"
#include "AL_EventGroup.h"
#include "AL_Task.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for RTOS tasks
 */
class Task : public Test::Base {
    // constructors
public:
    // delete default constructors
    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;

    /**
     * @brief get singleton instance
     */
    static Task& getInstance();

private:
    Task();

    RTOS::EventGroup grp;
    RTOS::Event      evtTaskReady;
    RTOS::Event      evtTaskStart;
    RTOS::Event      evtTaskStarted;
    RTOS::Event      evtTaskRun;
    RTOS::Event      evtTaskRunning;
    friend RTOS::Task<64, ::Test::Task>;
    RTOS::Task<64, ::Test::Task> rtosTask;

    // Test::Base implementation
    virtual void                         runInternal() final;
    virtual const std::list<Test::Base*> getPrerequisits() final;

    // RTOS::Task implementation
    void onStart();
    void onRun();

    /** singleton instance */
    static Task              instance;
    static const char* const name;

    static constexpr uint8_t kTaskPriority =
        1; /**< Priority of the task under test */
};
}  // namespace Test
#endif  //__TESTTASK_H__