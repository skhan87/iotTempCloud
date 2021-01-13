/**
 * @file TestTimer.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for timers
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __TESTTIMER_H__
#define __TESTTIMER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Timer;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Event.h"
#include "AL_EventGroup.h"
#include "AL_Timer.h"
#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for timers
 */
class Timer : public Test::Base, RTOS::Timer
{
    // constructors
public:
    // delete default constructors
    Timer(const Timer &other) = delete;
    Timer &operator=(const Timer &other) = delete;

    /**
     * @brief get singleton instance
     */
    static Timer &getInstance();

private:
    Timer();

    // Test::Base
    virtual void                    runInternal() final;
    virtual const std::list<Test::Base *> getPrerequisits() final;

    // RTOS::Timer
    virtual void onTimer() final;

    RTOS::EventGroup grp;
    RTOS::Event      evtOnTime;

    /** singleton instance */
    static Timer instance;
};
}  // namespace Test
#endif  //__TESTTIMER_H__