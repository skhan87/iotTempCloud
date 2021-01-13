/**
 * @file StateMachine.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief state pattern container
 * @version 1.0
 * @date 2020-03-19
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "StateMachine.h"
#include <utility>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

template<class TContext>
Patterns::StateMachine<TContext>::StateMachine(
    TContext &         context,
    Activity<TContext> startActivity) :
    context(context),
    currentActivity(nullptr), startActivity(startActivity)
{
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

template<class TContext>
void Patterns::StateMachine<TContext>::run()
{
    if(currentActivity == nullptr)
        currentActivity = startActivity;

    // run this, until finish state (nullptr) is returned.
    do
    {
        (context.*currentActivity)();
        currentActivity = getNextState(context, currentActivity);
    } while(currentActivity != nullptr);
}

template<class TContext>
TContext &Patterns::StateMachine<TContext>::getContext()
{
    return context;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------