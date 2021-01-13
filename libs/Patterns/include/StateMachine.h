/**
 * @file StateMachine.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief state pattern containers
 * @version 1.0
 * @date 2020-03-19
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Patterns
{
/**
 * @brief function that executes something within the given context
 *
 * @tparam TContext
 */
template<class TContext>
using Activity = void (TContext::*)();

template<class TContext>
class StateMachine;
}  // namespace Patterns

//--------------------------------- INCLUDES ----------------------------------

namespace Patterns
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief container for state pattern.
 * Contains states and controls them
 *
 * @tparam TContext type of the activities context
 */
template<class TContext>
class StateMachine
{
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    StateMachine()                          = delete;
    StateMachine(const StateMachine &other) = delete;
    StateMachine &operator=(const StateMachine &other) = delete;

protected:
    /**
     * @brief Construct a new Container object
     *
     * @param name
     * @param startActivity activity to start with. Supplied by child class
     */
    StateMachine(TContext &context, Activity<TContext> startActivity);
    // exposed functions
public:
    /**
     * @brief calls internal states and transitions
     *
     */
    void run();
    /**
     * @brief Get the Context
     *
     * @return TContext&
     */
    TContext &getContext();

    // private variables
private:
    /**
     * @brief context this container uses and passes to all its Activities.
     *
     */
    TContext &context;
    /**
     * @brief currently active activity
     *
     */
    Activity<TContext> currentActivity;
    /**
     * @brief save start activity for reentrance
     *
     */
    Activity<TContext> const startActivity;

protected:
    /**
     * @brief transition function to get the next state.
     *
     * @return Activity* state to switch to or NULL to terminate.
     */
    virtual Activity<TContext> getNextState(
        TContext &               context,
        const Activity<TContext> currentState) = 0;
};
}  // namespace Patterns

#include "../src/StateMachine.cpp"
#endif  //__CONTAINER_H__