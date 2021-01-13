/**
 * @file CCallable.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief wrapper for passing C++ member functions to C callbacks
 * @version 1.0
 * @date 2019-11-20
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "CCallable.h"
#include <utility>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

template<class T,
         class FctType,
         FctType T::*fct,
         class... FirstArgTypes,
         class... SecondArgTypes>
decltype(auto) Patterns::wrapCCallable(FirstArgTypes &&... args1,
                                       void *env,
                                       SecondArgTypes &&... args2)
{
    return (static_cast<T *>(env)->*fct)(
        std::forward<FirstArgTypes>(args1)...,
        std::forward<SecondArgTypes>(args2)...);
}

template<class ClassToBeSingle,
         class M,
         M ClassToBeSingle::*fct,
         class... ArgTypes>
decltype(auto) Patterns::wrapSingleton(ArgTypes &&... args)
{
    return (ClassToBeSingle::instance().*fct)(std::forward<ArgTypes>(args)...);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------