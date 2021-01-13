/**
 * @file TestEvent.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for events and event groups
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTEVENT_H__
#define __TESTEVENT_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Event;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Event.h"
#include "AL_EventGroup.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for events and event groups
 */
class Event : public Test::Base
{
    // constructors
public:
    // delete default constructors
    Event(const Event &other) = delete;
    Event &operator=(const Event &other) = delete;

    /** Singleton getInstance */
    static Event &getInstance();

private:
    Event();

protected:
    virtual void runInternal() final;

private:
    /** just tests triggering of one event */
    RTOS::EventGroup grp1;
    RTOS::Event      evt1;
    RTOS::Event      evt2;

    /** singleton instance */
    static Event instance;
};
}  // namespace Test
#endif  //__TESTEVENT_H__