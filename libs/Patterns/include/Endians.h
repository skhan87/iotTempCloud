/**
 * @file Endians.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief functions for working with endianess
 * @version 1.0
 * @date 2020-08-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __ENDIANS_H__
#define __ENDIANS_H__

//-------------------------------- PROTOTYPES ---------------------------------

// sanity checks
#ifndef __BYTE_ORDER__
    #error Byteorder not defined
#endif
#ifndef __ORDER_BIG_ENDIAN__
    #error big endian not defined
#endif
#ifndef __ORDER_LITTLE_ENDIAN__
    #error little endiian not defined
#endif
#if (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__) && \
    (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__)
    #error unknown endianess
#endif

//--------------------------------- INCLUDES ----------------------------------

namespace Endians
{
//-------------------------------- CONSTANTS ----------------------------------

/**
 * @brief enumeration values for endianess.
 * 
 */
enum ByteOrder {
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
};

//-------------------------------- FUNCTIONS ----------------------------------

template <class T>
constexpr void machineToBig(T& val);

template <class T>
constexpr void machineToLittle(T& val);

template <class T>
constexpr void bigToMachine(T& val);

template <class T>
constexpr void littleToMachine(T& val);
}  // namespace Endians

#include "../src/Endians.cpp"
#endif  //__ENDIANS_H__