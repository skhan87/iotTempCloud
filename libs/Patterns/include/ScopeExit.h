/**
 * @file ScopeExit.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Object that invokes a given function when going out of scope.
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __SCOPEEXIT_H__
#define __SCOPEEXIT_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include <tuple>

namespace Patterns
{
//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Object that invokes a given function when going out of scope.
 * 
 * @details The callable and the given arguments are saved internally
 * and then called when the destructor is called and the instance
 * has not been deactivated.
 * Moveable but not copyable.
 * 
 * @example Look in make_ScopeExit.
 * 
 */
template<class CallableT, class... CallableArgsT>
class ScopeExit {
    ScopeExit()                       = delete;
    ScopeExit(const ScopeExit& other) = delete;
    ScopeExit& operator=(const ScopeExit& other) = delete;

private:
    const CallableT              callable;
    std::tuple<CallableArgsT...> args;
    bool                         deactivated;

public:
    ScopeExit(CallableT callable, CallableArgsT&&... args);

    // include move operators
    ScopeExit(ScopeExit&& other) = default;
    ScopeExit& operator=(ScopeExit&& other) = default;

    ~ScopeExit();

    void deactivate();
};

template<class CallableT, class... CallableArgsT>
ScopeExit<CallableT, CallableArgsT...> make_scopeExit(CallableT callable,
                                                      CallableArgsT&&... args);
}  // namespace Patterns

#include "../src/ScopeExit.cpp"
#endif  //__SCOPEEXIT_H__