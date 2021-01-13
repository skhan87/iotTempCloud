/**
 * @file TestEvent.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for RTOS events and event groups
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestEvent.h"
#include "AL_Event.h"
#include "AL_EventGroup.h"
#include "AL_RTOS.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Event Test::Event::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Event::Event() : Base("RTOS", "Event"), grp1(), evt1(grp1), evt2(grp1)
{
}

Test::Event &Test::Event::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Event::runInternal()
{
    // just set and reset events
    evt1.trigger();
    assert(evt1.wasTriggered(), "failed to trigger single event sync");
    evt1.reset();
    assert(!evt1.wasTriggered(), "failed to reset single event sync");

    // wait for two events with and
    evt1.trigger();
    evt2.trigger();
    RTOS::EventList<2> lst({&evt1, &evt2});
    assert(grp1.await(lst, 10, RTOS::EventGroup::WaitMode::And)
               == Error::None,
           "Failed to wait on two events with EventGroup::await");
    grp1.resetEvents(lst);

    // wait for one out of two events
    evt2.trigger();
    assert(grp1.await(lst, 10, RTOS::EventGroup::WaitMode::Or)
               == Error::None,
           "Failed to wait on oneout of two events with EventGroup::await");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------