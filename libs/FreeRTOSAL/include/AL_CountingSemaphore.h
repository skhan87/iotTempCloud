/**
 * @file AL_CountingSemaphore.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction of a counting semaphore
 * @version 1.0
 * @date 2020-03-04
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_COUNTINGSEMAPHORE_H__
#define __AL_COUNTINGSEMAPHORE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
class CountingSemaphore;
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
 * @brief Used to synchronize tasks.
 * 
 * @warning For mutual exclusion use Mutex. This class is for
 * Synchronization.
 * 
 * @details Use with countMax=1 for a binary Semaphore.
 *
 */
class CountingSemaphore {
public:
    // delete default constructors
    CountingSemaphore(const CountingSemaphore& other) = delete;
    CountingSemaphore& operator=(const CountingSemaphore& other) = delete;

    CountingSemaphore(uint32_t countMax = (((BaseType_t)0) | ~((BaseType_t)0)),
                      uint32_t startCount = 0);
    ~CountingSemaphore();

public:
    Error::Code take(milliseconds timeoutMs = Infinity);
    Error::Code takeFromISR(bool* contextSwitchNeeded = nullptr);
    Error::Code give();
    Error::Code giveFromISR(bool* contextSwitchNeeded = nullptr);
    uint32_t    getCount();

private:
    /**
     * @brief used by FreeRTOs to handle this semaphore
     *
     */
    SemaphoreHandle_t handle;
    /**
     * @brief data needed by FreeRTOS for this semaphore
     *
     */
    StaticSemaphore_t semaphoreData;
};
}  // namespace RTOS
#endif  //__AL_COUNTINGSEMAPHORE_H__