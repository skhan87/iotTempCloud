/**
 * @file AL_Queue.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for queues
 * @version 1.0
 * @date 2020-03-09
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Queue.h"
#include <memory>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new Queue
 *
 * @param name String identifier for queue
 */
template<class T, size_t queueLength>
RTOS::Queue<T, queueLength>::Queue(const char* const name)
        : handle(xQueueCreateStatic(queueLength, sizeof(T), buffer, &data))
{
    vQueueAddToRegistry(handle, name);
}

/**
 * @brief Destroy the Queue
 *
 */
template<class T, size_t queueLength>
RTOS::Queue<T, queueLength>::~Queue()
{
    vQueueDelete(handle);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Adds object to the queue.
 * 
 * @warning Only use from task context.
 *
 * @param object RValue reference to object (has to be moved in).
 * @param timeoutMs Maximum time to wait for function to finish.
 * @return Error::Code Might fail with OutOfResources if queue is full.
 */
template<class T, size_t queueLength>
Error::Code RTOS::Queue<T, queueLength>::send(const T&&    object,
                                              milliseconds timeout)
{
    if (xQueueSend(handle,
                   static_cast<const void*>(&object),
                   Utility::millisToTicks(timeout)) == pdTRUE) {
        // Avoid calling the destructor
        std::move(object);
        return Error::None;
    } else {
        // queue full
        return Error::OutOfResources;
    }
}

/**
 * @brief Adds object to the queue from ISR.
 *
 * @param object RValue reference to object (has to be moved in).
 * @param contextSwitchNeeded Whether the scheduler has to be invoked.
 * @return Error::Code Might fail with OutOfResources if queue is full.
 */
template<class T, size_t queueLength>
Error::Code RTOS::Queue<T, queueLength>::sendFromISR(const T&& object,
                                                     bool* contextSwitchNeeded)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    auto       success                   = xQueueSendFromISR(handle,
                                     static_cast<const void*>(&object),
                                     &pxHigherPriorityTaskWoken) == pdTRUE;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = pxHigherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        // Avoid calling the destructor
        std::move(object);
        return Error::None;
    } else {
        // queue full
        return Error::OutOfResources;
    }
}

/**
 * @brief Receive next element from queue.
 * 
 * @warning Only use from task context.
 *
 * @param outObject Reference to put the received object in.
 * @param timeoutMs Maximum time to wait for function to finish.
 * @return Error::Code Might return Empty if no element is in queue.
 */
template<class T, size_t queueLength>
Error::Code RTOS::Queue<T, queueLength>::receive(T&           outObject,
                                                 milliseconds timeout)
{
    T buffer {};
    if (xQueueReceive(handle,
                      static_cast<void*>(&buffer),
                      Utility::millisToTicks(timeout)) == pdTRUE) {
        outObject = std::move(buffer);
        return Error::None;
    } else {
        // queue empty
        return Error::Empty;
    }
}

/**
 * @brief Receive next element from queue ISR.
 *
 * @param outObject Reference to put the received object in.
 * @param contextSwitchNeeded Whether the scheduler has to be invoked.
 * @return Error::Code Might return Empty if no element is in queue.
 */
template<class T, size_t queueLength>
Error::Code
    RTOS::Queue<T, queueLength>::receiveFromISR(T&    outObject,
                                                bool* contextSwitchNeeded)
{
    T          buffer {};
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    auto       success                   = xQueueReceiveFromISR(handle,
                                        static_cast<void*>(&buffer),
                                        &pxHigherPriorityTaskWoken) == pdTRUE;

    if (contextSwitchNeeded) {
        *contextSwitchNeeded = pxHigherPriorityTaskWoken == pdTRUE;
    }

    if (success) {
        outObject = std::move(buffer);
        return Error::None;
    } else {
        // queue empty
        return Error::Empty;
    }
}

/**
 * @brief Get string identifier of this queue.
 *
 * @return const char* const String identifier (name)
 */
template<class T, size_t queueLength>
const char* const RTOS::Queue<T, queueLength>::getName()
{
    return pcQueueGetName(handle);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------