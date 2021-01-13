/**
 * @file Observer.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief implements observer pattern for Observables
 * @version 1.0
 * @date 2019-12-09
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Observer.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

template <class... ObservableParamTypes>
Patterns::Observer<ObservableParamTypes...>::Observer(
    Observable<ObservableParamTypes...>& observable)
    : observable(observable)
{
    observable.registerObserver(*this);
}

template <class... ObservableParamTypes>
Patterns::Observer<ObservableParamTypes...>::~Observer()
{
    observable.unregisterObserver(*this);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------