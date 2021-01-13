/**
 * @file TestTask.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS tasks
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestTask.h"
#include <cstring>
#include "TestEvent.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Task        Test::Task::instance {};
const char* const Test::Task::name = "TestTask";

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Task::Task()
        : Test::Base("RTOS", "Task"), grp(), evtTaskReady(grp),
          evtTaskStart(grp), evtTaskStarted(grp), evtTaskRun(grp),
          evtTaskRunning(grp), rtosTask {*this, name, kTaskPriority}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Task::runInternal()
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

const std::list<Test::Base*> Test::Task::getPrerequisits()
{
    return std::list<Test::Base*>({&Test::Event::getInstance()});
}

void Test::Task::onStart()
{
    // gets resumed by runInternal() from another task
    rtosTask.suspend();
    evtTaskReady.trigger();
    evtTaskStart.await();
    evtTaskStarted.trigger();
}

void Test::Task::onRun()
{
    evtTaskRun.await();
    evtTaskRunning.trigger();
    rtosTask.suspend();
    // forever suspended
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------