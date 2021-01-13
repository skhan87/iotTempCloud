/**
 * @file TestQueue.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test RTOS queue
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestQueue.h"
#include <cstring>

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Queue       Test::Queue::instance {};
const char* const Test::Queue::name = "TestQueue";

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Queue::Queue() : Test::Base("RTOS", "Queue"), queue1(name) {}

Test::Queue& Test::Queue::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::Queue::runInternal()
{
    assert(strcmp(queue1.getName(), name) == 0,
           "wrong queue name %s",
           queue1.getName());
    uint64_t value = 0;
    assert(queue1.receive(value, 0) == Error::Empty,
           "empty queue returned value");
    assert(queue1.send(1234, 10) == Error::None,
           "failed to send value to queue");
    value = 0;
    assert(queue1.receive(value, 10) == Error::None, "failed to receive");
    assert(value == 1234, "wrong value received from queue");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------