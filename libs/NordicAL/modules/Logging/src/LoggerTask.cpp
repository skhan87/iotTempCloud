/**
 * @file LoggerTask.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief task that runs logging
 * @version 1.0
 * @date 2020-04-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#include <sdk_config.h>

#if NRF_LOG_ENABLED

//--------------------------------- INCLUDES ----------------------------------

#include "LoggerTask.h"
#include "PortUtility.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_internal.h"
#include "nrf_log_types.h"

#include <Error.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

/**
 * @brief Eager loading singleton instance.
 * 
 */
Log::Task Log::Task::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Singleton constructor for logging task
 * 
 */
Log::Task::Task()
        : eventGroup {}, logQueued {eventGroup}, rtosTask {*this,
                                                           "Logger",
                                                           kLogTasKPriority}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Get the singleton instance
 * 
 * @return Task& Single  logging task
 */
Log::Task& Log::Task::getInstance()
{
    return instance;
}

/**
 * @brief initialize the logging module
 *
 */
void Log::Task::init()
{
    CHECK_ERROR(Port::Utility::getError(NRF_LOG_INIT(NULL)));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief trigger flushing of the logging task
 * 
 */
void Log::Task::triggerLog()
{
    instance.logQueued.trigger();
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief Implementation of task context
 * 
 */
void Log::Task::onStart()
{
    // do nothing
}

/**
 * @brief Implementation of task context.
 * 
 * @details Wait's for logs and flushes logs when needed.
 * 
 */
void Log::Task::onRun()
{
    LOG_E_ON_ERROR(logQueued.await(), "Logging Task error");
    logQueued.reset();
    NRF_LOG_FLUSH();
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

#endif
