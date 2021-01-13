/**
 * @file LoggerTask.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief singleton task that runs logging
 * @version 1.0
 * @date 2020-04-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#include <sdk_config.h>

#if NRF_LOG_ENABLED

#ifndef __LOGGERTASK_H__
#define __LOGGERTASK_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Log
{
class Task;
}

//--------------------------------- INCLUDES ----------------------------------

#include <AL_Event.h>
#include <AL_EventGroup.h>
#include <AL_RTOS.h>
#include <AL_Task.h>

namespace Log
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief singleton task that runs logging.
 * Implemented as eager loading singleton so it will always start.
 */
class Task {
    static constexpr RTOS::milliseconds kLogTasKPriority =
        1; /**< Priority of the log task */
    static constexpr size_t kStackSize =
        512; /**< Stack size of the logging task in sizeof(StackType_t) bytes*/

    friend RTOS::Task<kStackSize, ::Log::Task>;

    // delete default constructors
    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;

public:
    static Task& getInstance();
    static void  init();
    static void  triggerLog();

private:
    Task();
    static Task instance;

    /** task implementation */
    void onStart();
    void onRun();

    RTOS::EventGroup eventGroup; /** Holds all logging related events */
    RTOS::Event
        logQueued; /**< gets triggered every time a log message is queued */
    RTOS::Task<kStackSize, ::Log::Task>
        rtosTask; /**< actual RTOS task that runs this context */
};
}  // namespace Log
#endif  //__LOGGERTASK_H__

#endif
