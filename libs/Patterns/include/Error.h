/**
 * @file Errors.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief basic error definitions and functions
 * @version
 * @date 2020-04-30
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __ERRORS_H__
#define __ERRORS_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <cstdint>
#include <cstdlib>

//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

#include <string.h>

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define CHECK_ERROR(err)                                  \
    Error::internalCheck(err,                             \
                         static_cast<uint32_t>(__LINE__), \
                         static_cast<const char* const>(__FILENAME__))

/**
 * @brief Macro that checks an error code and returns the current function if an error occured.
 * 
 */
#define RETURN_ON_ERROR(err)          \
    {                                 \
        Error::Code errCode = err;    \
        if (errCode != Error::None) { \
            return errCode;           \
        }                             \
    }

/**
 * @brief basic error definitions and functions.
 * Pure static class. Non instantiable.
 */
class Error {
    // delete default constructors
    Error()                   = delete;
    Error(const Error& other) = delete;
    Error& operator=(const Error& other) = delete;

public:
    // structs and enums
    /**
     * @brief error code.
     * Used to have a common way of error handling throughout the whole code
     * base.
     *
     */
    enum Code {
        None = 0,

        NotInitialized,
        NotFound,
        ChecksumFailed,
        AlreadyInit,
        UsingDestroyed,
        Alignment,
        TooLarge,
        OutOfResources,
        Memory,
        Internal,
        InvalidUse,
        Busy,
        StackCorrupt,
        InvalidParameter,
        Timeout,
        MinReached,
        MaxReached,
        Empty,
        Full,
        Lifetime,
        CommunicationFailed,
        PureVirtual,
        Acknowledgement,
        SizeMissmatch,

        Unknown,

        Count
    };

    /**
     * @brief tests an error code.
     * If code is anything else than success, restarts.
     * 
     * @warning do not directly use. Use CHECK_ERROR() instead.
     * 
     * @param code 
     */
    static void
        internalCheck(Code code, uint32_t lineNumber, const char* fileName);

    /**
     * @brief Get the Description string for the given error code.
     *
     * @warning Only use with #ifdef DEBUG safeguards. Function is only
     * available when compiling with DEBUG macro
     *
     * @param code
     * @return const char* const
     */
    static const char* const getDescription(Code code);

private:
    /**
     * @brief string error descriptors
     *
     */
    static const char* const Descriptions[];
};
#endif  //__ERRORS_H__
