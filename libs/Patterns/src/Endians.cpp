/**
 * @file Endians.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief functions for converting endianess
 * @version 1.0
 * @date 2020-08-19
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Endians.h"

#include <algorithm>
#include <cstdint>
#include <type_traits>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

// internal
namespace Endians
{
namespace detail
{
template <typename T>
auto is_iterable_impl(int) -> decltype(
    std::begin(std::declval<T&>()) !=
        std::end(std::declval<T&>()),  // begin/end and operator !=
    void(),                            // Handle evil operator ,
    ++std::declval<decltype(begin(std::declval<T&>()))&>(),  // operator ++
    void(*std::begin(std::declval<T&>())),                   // operator*
    std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

}  // namespace detail

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

template <class T>
constexpr
    typename std::enable_if<!is_iterable<T>::value && !std::is_array<T>::value,
                            void>::type
    swap(T& val)
{
    auto ptr = reinterpret_cast<uint8_t*>(&val);
    std::reverse(ptr, ptr + sizeof(T));
}

template <class T>
constexpr
    typename std::enable_if<is_iterable<T>::value || std::is_array<T>::value,
                            void>::type
    swap(T& val)
{
    // only wap endians for each element
    std::for_each(std::begin(val), std::end(val), [](auto& element) {
        Endians::swap(element);
    });
}
}  // namespace Endians

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Converts machine endianess to big endian
 * 
 * @tparam T 
 * @param val 
 * @return T 
 */
template <class T>
constexpr void Endians::machineToBig(T& val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // do nothing
#else
    Endians::swap(val);
#endif
}

/**
 * @brief Converts machine endianess to little endian
 * 
 * @tparam T 
 * @param val 
 * @return T 
 */
template <class T>
constexpr void Endians::machineToLittle(T& val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    Endians::swap(val);
#else
    // do nothing
#endif
}

/**
 * @brief Converts big endian to machine endianess.
 * 
 * @details Can be used when receiving data from network
 * to ensure the local is able to properly work with the value.
 * 
 * @tparam T 
 * @param val 
 * @return T 
 */
template <class T>
constexpr void Endians::bigToMachine(T& val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // do nothing
#else
    Endians::swap(val);
#endif
}

/**
 * @brief Converts little to machine endianess.
 * 
 * @tparam T 
 * @param val 
 * @return T 
 */
template <class T>
constexpr void Endians::littleToMachine(T& val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    Endians::swap(val);
#else
    // do nothing
#endif
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
