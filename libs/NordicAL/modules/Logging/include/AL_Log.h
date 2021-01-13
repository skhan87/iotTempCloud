/**
 * @file AL_Log.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for logging module
 * @version 1.0
 * @date 2020-04-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_LOG_H__
#define __AL_LOG_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include "LoggerTask.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "Error.h"

//---------------------------------- MACROS -----------------------------------

#if NRF_LOG_ENABLED
// forward to logging

#define LOG_E(...)                  \
    {                               \
        NRF_LOG_ERROR(__VA_ARGS__); \
        Log::Task::triggerLog();    \
    }
#define LOG_W(...)                    \
    {                                 \
        NRF_LOG_WARNING(__VA_ARGS__); \
        Log::Task::triggerLog();      \
    }
#define LOG_I(...)                 \
    {                              \
        NRF_LOG_INFO(__VA_ARGS__); \
        Log::Task::triggerLog();   \
    }
#define LOG_D(...)                  \
    {                               \
        NRF_LOG_DEBUG(__VA_ARGS__); \
        Log::Task::triggerLog();    \
    }

#define LOG_RAW_I(...)                 \
    {                                  \
        NRF_LOG_RAW_INFO(__VA_ARGS__); \
        Log::Task::triggerLog();       \
    }

#define LOG_E_ON_ERROR(errorCode, ...)  \
    {                                   \
        if (errorCode != Error::None) { \
            LOG_E(__VA_ARGS__);         \
        }                               \
    }

#define LOG_W_ON_ERROR(errorCode, ...)  \
    {                                   \
        if (errorCode != Error::None) { \
            LOG_W(__VA_ARGS__);         \
        }                               \
    }

#define LOG_FLUSH NRF_LOG_FLUSH

#else
// dump, no logging

#define LOG_E(...)
#define LOG_W(...)
#define LOG_I(...)
#define LOG_D(...)

#define LOG_RAW_I(...)

#define LOG_E_ON_ERROR(errorCode, ...)
#define LOG_W_ON_ERROR(errorCode, ...)

#define LOG_FLUSH
#endif

#endif  //__AL_LOG_H__