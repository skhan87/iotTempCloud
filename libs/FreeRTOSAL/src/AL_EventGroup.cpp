/**
 * @file AL_EventGroup.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for event groups
 * @version 1.0
 * @date 2020-03-06
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_EventGroup.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

RTOS::EventGroup::EventGroup() :
    handle(xEventGroupCreateStatic(&data)), allocatedEvents(0)
{
}

RTOS::EventGroup::~EventGroup()
{
    vEventGroupDelete(handle);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

const EventBits_t RTOS::EventGroup::alloc()
{
    EventBits_t bit   = 1u;
    uint8_t     index = 0;
    while(allocatedEvents & (1u << index))
    {
        ++index;
        bit = (1u << index);
        if((int)index >= (sizeof(EventBits_t) * 8))
        {
            // error
            return 0;
        }
    }
    allocatedEvents |= bit;
    return bit;
}

void RTOS::EventGroup::free(const EventBits_t bit)
{
    allocatedEvents &= ~(bit);
}

//---------------------------- STATIC FUNCTIONS -------------------------------