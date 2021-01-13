/**
 * @file AL_Mutex.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for mutex
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_MUTEX_H__
#define __AL_MUTEX_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class Mutex;
}

//--------------------------------- INCLUDES ----------------------------------

#include <FreeRTOS.h>
#include <Error.h>
#include "AL_RTOS.h"
#include "FreeRTOSUtility.h"
#include "semphr.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Mutual Exclusion object.
 * Can be used to lock single use resources.
 *
 */
class Mutex
{
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    Mutex(const Mutex &other) = delete;
    Mutex &operator=(const Mutex &other) = delete;

    /**
     * @brief Construct a new, unlocked Mutex
     *
     */
    Mutex();
    /**
     * @brief Destroy the Mutex objectMutex destructor
     *
     */
    ~Mutex();

    // exposed functions
public:
    /**
     * @brief try to lock the mutex.
     * Waits up to timeoutMs.
     *
     * @param timeoutMs
     * @return Error::Timeout if mutex could not be obtained within timeoutMs
     */
    Error::Code tryObtain(milliseconds timeoutMs = Infinity);
    /**
     * @brief tries to obtain mutex from ISR.
     * Does not block, because it is meant to be used in ISR.
     *
     * @param contextSwtichNeeded whether the calling ISR should init a context
     * switch
     * @return Error::OutOfResources when mutex is already occupied
     */
    Error::Code tryObtainFromISR(bool *contextSwitchNeeded = nullptr);
    /**
     * @brief tries to release the semaphore.
     *
     * @return Error::InvalidUse when current task is not the one that obtained the mutex
     */
    Error::Code tryRelease();
    /**
     * @brief tries to release the mutex from an ISR.
     * Does not block because it is to be used from ISR.
     *
     * @param contextSwitchNeeded whether calling ISR must initiate a context
     * switch.
     * @return Error::Unknown
     */
    Error::Code tryReleaseFromISR(bool *contextSwitchNeeded = nullptr);
    /**
     * @brief returns true if mutex is locked at the moment
     *
     * @return true
     * @return false
     */
    bool isLocked();

    // interface implementation
public:
    // private variables
private:
    /**
     * @brief handle FreeRTOS uses to identify Mutex
     *
     */
    SemaphoreHandle_t handle;

    /**
     * @brief data freertos needs to handle the semaphore
     *
     */
    StaticSemaphore_t semaphoreData;

    // private functions
private:
    // static functions
private:
protected:
};
}  // namespace RTOS
#endif  //__AL_MUTEX_H__