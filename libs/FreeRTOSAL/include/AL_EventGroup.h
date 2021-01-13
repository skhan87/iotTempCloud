/**
 * @file AL_EventGroup.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for Event groups
 * @version 1.0
 * @date 2020-03-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_EVENTGROUP_H__
#define __AL_EVENTGROUP_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class EventGroup;
}

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include <event_groups.h>
#include <array>
#include <Error.h>
#include "AL_Event.h"
#include "AL_RTOS.h"
#include "FreeRTOSUtility.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Group of events.
 * Application can wait for multiple events in each group.
 *
 */
class EventGroup
{
    /**
     * @brief befriend event for access to private FreeRTOS members
     *
     */
    friend class Event;

    // structs and enums
public:
    /**
     * @brief how to wait for multiple Events for the group
     *
     */
    enum class WaitMode
    {
        /**
         * @brief waiting for any of the flags to be set
         *
         */
        Or,
        /**
         * @brief waiting for all of the flags to be set
         *
         */
        And,
    };

    // constructors
public:
    EventGroup(const EventGroup &other) = delete;
    EventGroup &operator=(const EventGroup &other) = delete;
    /**
     * @brief Construct a new Event Group
     *
     */
    EventGroup();
    /**
     * @brief Destroy the Event Group
     *
     */
    ~EventGroup();

    // exposed functions
public:
    /**
     * @brief wait for given events
     *
     * @tparam numberEvents
     * @param eventsToWait list of events to wait for
     * @param timeoutMs maximum time to wait
     * @param mode whether to wait for one or multiple events
     * @return Error::InvalidParameter if on of the events does not belong to this group
     * @return Error::Timeout if timeout passed without the trigger conditions beeing met
     */
    template<size_t numberEvents>
    Error::Code await(EventList<numberEvents> eventsToWait,
                     milliseconds            timeout = Infinity,
                     WaitMode                mode    = WaitMode::And)
    {
        // all bits to wait for
        EventBits_t bitsToWait = 0u;
        for(const auto *event: eventsToWait)
        {
            if(event->group != this)
            {
                // one of the given Events does not belong to this group
                return Error::InvalidParameter;
            }
            bitsToWait |= event->bit;
        }

        BaseType_t xClearOnExit = pdFALSE;
        BaseType_t xWaitForAllBits =
            mode == RTOS::EventGroup::WaitMode::And ? pdTRUE : pdFALSE;
        EventBits_t bitsAfterWait =
            xEventGroupWaitBits(handle,
                                reinterpret_cast<const EventBits_t>(bitsToWait),
                                xClearOnExit,
                                xWaitForAllBits,
                                Utility::millisToTicks(timeout));

        switch(mode)
        {
            case RTOS::EventGroup::WaitMode::And:
                // all events should have triggered
                if((bitsAfterWait & bitsToWait) == bitsToWait)
                {
                    return Error::None;
                } else
                {
                    return Error::Timeout;
                }
            case RTOS::EventGroup::WaitMode::Or:
                // one event should have triggered
                if((bitsAfterWait & bitsToWait) != 0)
                {
                    return Error::None;
                } else
                {
                    return Error::Timeout;
                }
            default:
                return Error::InvalidParameter;
        }
    }

    /**
     * @brief resets the given events in this group
     *
     * @tparam numberEvents
     * @param events
     * @return Error::InvalidParameter if one of the events does not belong to this event group
     */
    template<size_t numberEvents>
    Error::Code resetEvents(EventList<numberEvents> events)
    {
        // all bits to wait for
        EventBits_t bitsToClear = 0u;
        for(const auto *event: events)
    {
            if(event->group != this)
            {
                // one of the given Events does not belong to this group
                return Error::InvalidParameter;
            }
            bitsToClear |= event->bit;
        }
        xEventGroupClearBits(handle,
                             reinterpret_cast<const EventBits_t>(bitsToClear));
        return Error::None;
    }

    // interface implementation
public:
    // private variables
private:
    /**
     * @brief FreeRTOS event group handle
     *
     */
    EventGroupHandle_t handle;
    /**
     * @brief FreeRTOS event group data
     *
     */
    StaticEventGroup_t data;
    /**
     * @brief bitmap of events already alocated in this event group.
     * bit is 1 if the event is allocated.
     *
     */
    EventBits_t allocatedEvents;

    // private functions
private:
public:
    /**
     * @brief allocate on event bit
     *
     * @return const EventBits_t
     */
    const EventBits_t alloc();
    /**
     * @brief free the given event bit
     *
     * @param bit
     */
    void free(const EventBits_t bit);
    // static functions
private:
protected:
};
}  // namespace RTOS
#endif  //__AL_EVENTGROUP_H__