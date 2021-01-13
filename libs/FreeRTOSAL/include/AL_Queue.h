/**
 * @file AL_Queue.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for queues
 * @version 1.0
 * @date 2020-03-09
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_QUEUE_H__
#define __AL_QUEUE_H__

//-------------------------------- PROTOTYPES ---------------------------------

#include <stddef.h>

namespace RTOS
{
template<class T, size_t queueLength>
class Queue;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_RTOS.h"
#include "FreeRTOSUtility.h"

#include <Error.h>
#include <FreeRTOS.h>
#include <queue.h>

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Queue for inter process communication
 *
 * @tparam T type of data segement in queue
 * @tparam queueLength number of T object that can be stored in queue
 */
template<class T, size_t queueLength>
class Queue {
public:
    // delete default constructors
    Queue()                   = delete;
    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;

    Queue(const char* const name);
    ~Queue();
    Error::Code       send(const T&& object, milliseconds timeoutMs = Infinity);
    Error::Code       sendFromISR(const T&& object,
                                  bool*     contextSwitchNeeded = nullptr);
    Error::Code       receive(T& outObject, milliseconds timeoutMs = Infinity);
    Error::Code       receiveFromISR(T&    outObject,
                                     bool* contextSwitchNeeded = nullptr);
    const char* const getName();
    // private variables
private:
    /**
     * @brief FreeRTOS queue handle
     *
     */
    QueueHandle_t handle;
    /**
     * @brief FreeRTOS data for a queue
     *
     */
    StaticQueue_t data;
    /**
     * @brief buffer used by FreeRTOS to copy stored objects to.
     *
     */
    uint8_t buffer[sizeof(T) * queueLength];
};
}  // namespace RTOS

// template cpp needs to be included from here, not from Makefile
#include "../src/AL_Queue.cpp"
#endif  //__AL_QUEUE_H__