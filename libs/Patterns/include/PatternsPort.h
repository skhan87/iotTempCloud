/**
 * @file PatternsPort.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief port specific functions that need to be supplied by the platform abstraction layer
 * @version 1.0
 * @date 2020-05-05
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __PATTERNSPORT_H__
#define __PATTERNSPORT_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <cstdint>

/**
 * @brief port specific functions.
 * 
 * @warning Only the header file is in the Patterns library.
 * Implement your own cpp in Platform AL!
 * 
 */
namespace Port
{
//-------------------------------- CONSTANTS ----------------------------------

//-------------------------------- FUNCTIONS ----------------------------------

/**
 * @brief restarts the MCU.
 * 
 */
void restart();

/**
 * @brief logging a single string.
 * Not a format string.
 * 
 * @param str 
 */
void logInfo(const char* const str);

/**
 * @brief forwards logging to actual implementation
 * 
 * @param line 
 * @param fileName 
 * @param errorDescription
 */
void logError(const char* const fileName,
              uint32_t          lineNumber,
              const char* const errorDescription);

/**
 * @brief disables all interrupts so error handling does not get interrupted.
 * 
 */
void disableInterrupts();

/**
 * @brief	Use to forcibly stop execution at a certain point.
 * 
 */
void faultBreakpoint();

}  // namespace Port
#endif  //__PATTERNSPORT_H__