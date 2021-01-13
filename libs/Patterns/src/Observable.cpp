/**
 * @file Observable.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief triggers observers
 * @version 1.0
 * @date 2019-10-26
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Observable.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

template <class... ObservableParamTypes>
void Patterns::Observable<ObservableParamTypes...>::registerObserver(
    Observer<ObservableParamTypes...>& observer) noexcept
{
    observers.push_front(&observer);
}

template <class... ObservableParamTypes>
void Patterns::Observable<ObservableParamTypes...>::unregisterObserver(
    Observer<ObservableParamTypes...>& observer) noexcept
{
    observers.remove(&observer);
}

template <class... ObservableParamTypes>
void Patterns::Observable<ObservableParamTypes...>::trigger(
    ObservableParamTypes... args)
{
    // call every single observer
    for (auto observer : observers) {
        observer->handle(*this, std::forward<ObservableParamTypes>(args)...);
    }
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------