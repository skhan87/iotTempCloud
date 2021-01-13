/**
 * @file stackProtector.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief overwrites newlib functions to execute as wanted in embedded env.
 * @version 0.1
 * @date 2020-05-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include <Error.h>
#include <FreeRTOS.h>
#include <task.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

/**
 * @brief Forward exit call to Error handling
 *
 */
extern "C" void _exit(int status)
{
    if (status == 127) {
        // newlib exits with 127 when stack protection check fails
        CHECK_ERROR(Error::StackCorrupt);
    } else {
        // this schould never happen, investigate!!
        CHECK_ERROR(Error::Unknown);
    }
}

/**
 * @brief Catches error when trying to invoke a pure virtual function.
 * 
 */
extern "C" void __cxa_pure_virtual()
{
    CHECK_ERROR(Error::PureVirtual);
}

/**
 * @brief Locks before malloc or free is used.
 * 
 */
extern "C" void __malloc_lock()
{
    vTaskSuspendAll();
};

/**
 * @brief Frees lock after malloc or free were used.
 * 
 */
extern "C" void __malloc_unlock()
{
    (void)xTaskResumeAll();
};

//-------------------------------- CONSTANTS ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------
