/**
 * @file Future.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Contains declarations that compensate for missing C++ version from the future.
 * @version 1.0
 * @date 2020-10-15
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __FUTURE_H__
#define __FUTURE_H__

//--------------------------------- INCLUDES ----------------------------------

#include <functional>

namespace Future
{
/**
 * @brief Poor mans C++14 compatible invoke
 */
template<typename Fn,
         typename... Args,
         std::enable_if_t<std::is_member_pointer<std::decay_t<Fn>> {}, int> = 0>
constexpr decltype(auto) invoke(Fn&& f, Args&&... args) noexcept(
    noexcept(std::mem_fn(f)(std::forward<Args>(args)...)))
{
    return std::mem_fn(f)(std::forward<Args>(args)...);
}

/**
 * @brief Poor mans C++14 compatible invoke
 */
template<typename Fn,
         typename... Args,
         std::enable_if_t<!std::is_member_pointer<std::decay_t<Fn>> {}, int> = 0>
constexpr decltype(auto) invoke(Fn&& f, Args&&... args) noexcept(
    noexcept(std::forward<Fn>(f)(std::forward<Args>(args)...)))
{
    return std::forward<Fn>(f)(std::forward<Args>(args)...);
}
}  // namespace Future
#endif  //__FUTURE_H__