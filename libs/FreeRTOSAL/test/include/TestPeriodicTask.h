/**
 * @file TestPeriodicTask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for Periodic RTOS tasks
 * @version 1.0
 * @date 2020-10-29
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTPERIODICTASK_H__
#define __TESTPERIODICTASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include "AL_EventGroup.h"
#include "AL_Event.h"
#include "AL_PeriodicTask.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Test for Periodic RTOS tasks
 */
class PeriodicTask : public Test::Base {
    // constructors
public:
    // delete default constructors
    PeriodicTask(const PeriodicTask& other) = delete;
    PeriodicTask& operator=(const PeriodicTask& other) = delete;

    /**
    * @brief get singleton instance
    */
    static PeriodicTask& getInstance();

private:
    PeriodicTask();

    RTOS::EventGroup grp;
    RTOS::Event      evtTaskReady;
    RTOS::Event      evtTaskStart;
    RTOS::Event      evtTaskStarted;
    RTOS::Event      evtTaskRun;
    RTOS::Event      evtTaskRunning;
    friend RTOS::PeriodicTask<64, ::Test::PeriodicTask>;
    RTOS::PeriodicTask<64, ::Test::PeriodicTask> rtosTask;

    virtual void                         runInternal() final;
    virtual const std::list<Test::Base*> getPrerequisits() final;

    // RTOS::Task implementation
    void onStart();
    void onTime();

    /** singleton instance */
    static PeriodicTask      instance;
    static const char* const name;

    static constexpr uint8_t kTaskPriority =
        1; /**< Priority of the task under test */
    static constexpr RTOS::milliseconds kInterval =
        1000; /**< interval used for periodic task */
};
}  // namespace Test
#endif  //__TESTPERIODICTASK_H__