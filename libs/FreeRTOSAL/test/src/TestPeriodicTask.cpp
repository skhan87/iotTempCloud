/**
 * @file TestPeriodicTask.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for PeriodicTask 
 * @version 1.0
 * @date 2020-10-29
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestPeriodicTask.h"
#include "TestEvent.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::PeriodicTask Test::PeriodicTask::instance {};
const char* const  Test::PeriodicTask::name = "TestTask";

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::PeriodicTask::PeriodicTask()
        : Test::Base("RTOS", "PeriodicTask"), grp(), evtTaskReady(grp),
          evtTaskStart(grp), evtTaskStarted(grp), evtTaskRun(grp),
          evtTaskRunning(grp), rtosTask {*this, name, kTaskPriority, kInterval}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void Test::PeriodicTask::runInternal()
{
    // task should have suspended itself
    const char* const str = rtosTask.getName();
    assert(strcmp(name, str) == 0, "getName returned: \"%s\"", str);
    assert(!evtTaskReady.wasTriggered(), "left suspended mode unexpectitly");
    rtosTask.resume();
    assert(evtTaskReady.await(10) == Error::None,
           "did not enter taskReady mode");
    evtTaskStart.trigger();
    assert(evtTaskStarted.await(10) == Error::None,
           "did not react to start event");
    evtTaskRun.trigger();
    assert(evtTaskRunning.await(10) == Error::None, "did not react to run");
}

const std::list<Test::Base*> Test::PeriodicTask::getPrerequisits()
{
    return std::list<Test::Base*>({&Test::Event::getInstance()});
}

void Test::PeriodicTask::onStart()
{
    // gets resumed by runInternal() from another task
    rtosTask.suspend();
    evtTaskReady.trigger();
    evtTaskStart.await();
    evtTaskStarted.trigger();
}

void Test::PeriodicTask::onTime()
{
    evtTaskRun.await();
    evtTaskRunning.trigger();
    rtosTask.suspend();
    // forever suspended
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------