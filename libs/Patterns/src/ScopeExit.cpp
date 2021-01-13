/**
 * @file ScopeExit.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Stack guard for triggering functions on leaving scope.
 * @version 1.0
 * @date 2020-10-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "ScopeExit.h"
#include "Future.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new ScopeExit.
 * 
 * @warning When using C++14 or lower use make_stackExit().
 * Otherwise the automatic template deduction does not work.
 * 
 * @details Create a ScopeExit instance to call a callable
 * object when the ScopeExit instance goes out of scope.
 * 
 * When used stack allocated, this enables stack guard function.
 * So you do not have to bother how or function exits, it can
 * be cleaned up by calling the right functions (e.g. releasing
 * a mutex) from here.
 * 
 * @example You can use this to create a ScopeExit object for different
 * callable objects:
 * 
 * ```cpp
 * void run() {
 *     ScopeExit scopeExit{[](){LOG_I("seems like run termintated");}};
 *     // will be printed now, no matter how you leave run
 * }
 * 
 * void mutexes() {
 *     auto& myMutex = getMyMutex();
 *     RETURN_ON_ERROR(myMutex.tryObtain());
 *     ScopeExit mutexExit{&RTOS::Mutex::tryRelease, myMutex};
 *     // mutex will be released, no matter how you exit the function
 * }
 * ``` 
 * 
 * @tparam CallableT Type of callable object
 * @tparam CallableArgsT Packed argument types
 * @param callable Object that will be invoked on destruction
 * @param args Arguments that will be passed to object when it is invoked
 */
template<class CallableT, class... CallableArgsT>
Patterns::ScopeExit<CallableT, CallableArgsT...>::ScopeExit(
    CallableT callable,
    CallableArgsT&&... args)
        : callable(callable), args(args...), deactivated(false)
{}

/**
 * @brief Invokes the callable object when this instance was not deactivated.
 * 
 * @tparam CallableT Type of callable object
 * @tparam CallableArgsT Packed argument types
 */
template<class CallableT, class... CallableArgsT>
Patterns::ScopeExit<CallableT, CallableArgsT...>::~ScopeExit()
{
    if (!deactivated) {
        Future::invoke(callable, std::get<CallableArgsT>(args)...);
    }
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Deactivate this instance of ScopeExit so it will not invoke its callable on destruction.
 * 
 * @tparam CallableT Type of callable object
 * @tparam CallableArgsT Packed argument types
 */
template<class CallableT, class... CallableArgsT>
void Patterns::ScopeExit<CallableT, CallableArgsT...>::deactivate()
{
    deactivated = true;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Wrapper for making ScopeExit objects.
 * 
 * @warning Only need up to C++14. C++17 and above
 * can directly use ScopeExit constructor with automatic
 * template deduction.
 * 
 * @details Wrapping of the constructor is needed, because C++14
 * is not able to do automatic template deduction on constructors.
 * ScopeExit will call callable with given args on destruction.
 * 
 * When used stack allocated, this enables stack guard function.
 * So you do not have to bother how or function exits, it can
 * be cleaned up by calling the right functions (e.g. releasing
 * a mutex) from here.
 * 
 * @example You can use this to create a ScopeExit object for different
 * callable objects:
 * 
 * ```cpp
 * void run() {
 *     auto scopeExit = make_scopeExit([](){LOG_I("seems like run termintated");});
 *     // will be printed now, no matter how you leave run
 * }
 * 
 * void mutexes() {
 *     auto& myMutex = getMyMutex();
 *     RETURN_ON_ERROR(myMutex.tryObtain());
 *     auto mutexExit = make_scopeExit(&RTOS::Mutex::tryRelease, myMutex);
 *     // mutex will be released, no matter how you exit the function
 * }
 * ``` 
 * 
 * @tparam CallableT automatically deduced type of callable.
 * @tparam CallableArgsT automatically deduced type of arguements.
 * @param callable Callable object to invoke on destruction.
 * @param args Arguments to invoke callable with.
 * @return ScopeExit Store on stack. Will trigger when destroyed.
 */
template<class CallableT, class... CallableArgsT>
Patterns::ScopeExit<CallableT, CallableArgsT...>
    Patterns::make_scopeExit(CallableT callable, CallableArgsT&&... args)
{
    return ScopeExit<CallableT, CallableArgsT...> {
        callable,
        std::forward<CallableArgsT>(args)...};
}