/**
 * @file PatternsPort.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief implements the functions required by Patterns module
 * @version 1.0
 * @date 2020-05-05
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "PatternsPort.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_sdh.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//-------------------------------- FUNCTIONS ----------------------------------

void Port::restart()
{
    // just restart
    NVIC_SystemReset();
}

void Port::logInfo(const char* const str)
{
    // reallocate string in static buffer so stack
    // allocated strings do not go out of scope
    NRF_LOG_RAW_INFO("%s", str);
    NRF_LOG_FLUSH();
}

void Port::logError(const char* const fileName,
                    uint32_t          lineNumber,
                    const char* const errorDescription)
{
    NRF_LOG_ERROR("%s:%u:\t%s", fileName, lineNumber, errorDescription);
    NRF_LOG_FINAL_FLUSH();
}

void Port::disableInterrupts()
{
    __disable_irq();
}

void Port::faultBreakpoint()
{
    NRF_BREAKPOINT_COND;
}