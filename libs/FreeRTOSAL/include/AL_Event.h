/**
 * @file AL_Event.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction of a single event
 * @version 1.0
 * @date 2020-03-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_EVENT_H__
#define __AL_EVENT_H__

//-------------------------------- PROTOTYPES ---------------------------------

#include <array>

namespace RTOS
{
class Event;
/**
 * @brief List of events to wait for
 *
 * @tparam N
 */
template <size_t N>
using EventList = std::array<const Event*, N>;
}  // namespace RTOS

//--------------------------------- INCLUDES ----------------------------------

#include "AL_EventGroup.h"
#include "FreeRTOSUtility.h"

#include <Error.h>
#include <FreeRTOS.h>

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief multiple events are bundled together in event groups
 *
 */
class Event {
    // befriend EventGroup to access bit
    friend class EventGroup;
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    Event()                   = delete;
    Event(const Event& other) = delete;
    Event& operator=(const Event& other) = delete;

    /**
     * @brief Construct a new Event inside event group
     *
     * @param group
     */
    Event(EventGroup& group);
    /**
     * @brief Destroy the Event
     *
     */
    ~Event();

    // exposed functions
public:
    /**
     * @brief triggers this event
     *
     */
    void trigger();
    /**
     * @brief triggers the event from interrupt service routine
     *
     * @param contextSwitchNeeded
     * @return true success
     * @return false timer service queue was full
     */
    Error::Code triggerFromISR(bool* contextSwitchNeeded = nullptr);
    /**
     * @brief reset this event
     *
     */
    void reset();
    /**
     * @brief reset this event from an interrupt service routine
     *
     */
    void resetFromISR();
    /**
     * @brief if this event was triggered
     *
     * @return true
     * @return false
     */
    bool wasTriggered();
    /**
     * @brief if this event was triggered from interrupt service routine
     *
     * @return true
     * @return false
     */
    bool wasTriggeredFromISR();
    /**
     * @brief wait for only this event.
     * Internally uses EventGroup::await.
     * Do not use from ISR!
     *
     * @param eventsToWait
     * @param timeoutMs
     * @return Error::Timeout if timeout passed without event having triggered
     */
    Error::Code await(milliseconds timeout = Infinity);

    // interface implementation
public:
    // private variables
private:
    /**
     * @brief group this event belongs to
     *
     */
    EventGroup* group;
    /**
     * @brief event index in the FreeRTOS EventBits_t
     *
     */
    const EventBits_t bit;

    // private functions
private:
    // static functions
private:
protected:
};
}  // namespace RTOS
#endif  //__AL_EVENT_H__