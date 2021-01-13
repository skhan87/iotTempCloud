/**
 * @file Errors.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief basic error handling implementation
 * @version 1.0
 * @date 2020-04-30
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Error.h"
#include <PatternsPort.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

const char* const Error::Descriptions[] = {
    [None]           = "operation successful",
    [NotInitialized] = "module or object not initialized",
    [NotFound]       = "resource(s) not found",
    [ChecksumFailed] = "a checksum did not match",
    [AlreadyInit]    = "resource already initialized",
    [UsingDestroyed] = "trying to use a already freed/destroyed resource",
    [Alignment]      = "a value is not properly aligned to word or buffer",
    [TooLarge]       = "supplied or requested value is too large",
    [OutOfResources] = "there are no more of the requested resources",
    [Memory]         = "memory was accessed in invalid way",
    [Internal] = "internal error in precompiled module where the code can "
                 "not be accessed",
    [InvalidUse]       = "Invalid use of a function",
    [Busy]             = "resource is busy",
    [StackCorrupt]     = "stackframe got corrupted (Maybe array out of bounds)",
    [InvalidParameter] = "parameter supplied to function are invalid",
    [Timeout]          = "time given for async operation ran out",
    [MinReached]       = "a minimum value has been undershoot",
    [MaxReached]       = "a maximum value has been overshoot",
    [Empty]            = "trying to perform operation on empty container",
    [Full]             = "trying to perform operation on full container",
    [Lifetime]         = "lifetime conditions of an object were violated",
    [CommunicationFailed] = "communication partner does not react",
    [PureVirtual]         = "you tried to invoke a pure virtaul function",
    [Acknowledgement]     = "communication device failed to ack mandatory byte",
    [SizeMissmatch]       = "size did not exactly match expected value",

    [Unknown] = "Unknown. This must never happen!"};

//------------------------------ CONSTRUCTOR ----------------------------------

//------------------------ EXPOSED STATIC FUNCTIONS ---------------------------

const char* const Error::getDescription(Code code)
{
    if (code >= Code::Count) {
        // invalid error code
        return nullptr;
    }
    return Descriptions[static_cast<int>(code)];
}

//------------------------ PRIVATE STATIC FUNCTIONS ---------------------------

#ifndef DEBUG
/**
     * @brief tests an error code.
     * If code is anything else than success, restarts.
     * 
     * @warning do not directly use. Use CHECK_ERROR() instead.
     * 
     * @param code 
     */
void Error::internalCheck(Code code, uint32_t lineNumber, const char* fileName)
{
    if (code != Error::None) {
        Port::disableInterrupts();

        Port::logError(fileName, lineNumber, getDescription(code));

        // kill the MCU and force restart
        Port::restart();
    }
}
#else
/**
     * @brief tests an error code.
     * If code is anything else than success, restarts.
     * 
     * @warning do not directly use. Use CHECK_ERROR() instead.
     * 
     * @param code 
     */
void Error::internalCheck(Code code, uint32_t lineNumber, const char* fileName)
{
    if (code != Error::None) {
        Port::disableInterrupts();
        // report
        Port::logError(fileName, lineNumber, getDescription(code));

        // stop program execution to look around
        Port::faultBreakpoint();

        // kill the MCU and force restart
        while (true) {
            // stay here forever
        };
    }
}
#endif
