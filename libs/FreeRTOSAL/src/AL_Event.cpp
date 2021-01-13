/**
 * @file AL_Event.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for events
 * @version 1.0
 * @date 2020-03-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Event.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

RTOS::Event::Event(EventGroup& group) : group(&group), bit(group.alloc())
{
    xEventGroupClearBits(group.handle, bit);
}

RTOS::Event::~Event()
{
    // free in group variable
    xEventGroupClearBits(group->handle, bit);
    group->free(bit);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void RTOS::Event::trigger()
{
    xEventGroupSetBits(group->handle, bit);
}

Error::Code RTOS::Event::triggerFromISR(bool* contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken;
    auto       success =
        xEventGroupSetBitsFromISR(group->handle,
                                  bit,
                                  &pxHigherPriorityTaskWoken) == pdPASS;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = pxHigherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        return Error::None;
    } else {
        return Error::OutOfResources;
    }
}

void RTOS::Event::reset()
{
    xEventGroupClearBits(group->handle, bit);
}

void RTOS::Event::resetFromISR()
{
    xEventGroupClearBitsFromISR(group->handle, bit);
}

bool RTOS::Event::wasTriggered()
{
    return (xEventGroupGetBits(group->handle) & bit) != 0;
}

bool RTOS::Event::wasTriggeredFromISR()
{
    return (xEventGroupGetBitsFromISR(group->handle) & bit) != 0;
}

Error::Code RTOS::Event::await(milliseconds timeout)
{
    // only wait for this
    return group->await(EventList<1>({this}), timeout);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------