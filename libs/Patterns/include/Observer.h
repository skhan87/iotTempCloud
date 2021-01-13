/**
 * @file Observer.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstract class to implementing the observer pattern for Observables.
 * @version 1.0
 * @date 2019-10-26
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

#ifndef __OBSERVER_H__
#define __OBSERVER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Patterns
{
template<class... ObservableParamTypes>
class Observer;
}

//--------------------------------- INCLUDES ----------------------------------

#include "Observable.h"

namespace Patterns
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief handles Observables
 *
 * @warning Does not implement inter process commmunication or conntext
 * switches. You can use RTOS synchronisation features in combination with this
 * to achieve that. Keep in mind, the Observable keeps a dynamic list
 * of Observers, so complexity is O(n) and also scales with Observer function
 * complexity.
 * 
 * @tparam ObservableParamTypes type of emitted arguments.
 */
template<class... ObservableParamTypes>
class Observer {
    // structs and enums
public:
    // constructors
protected:
    /**
     * @brief Construct a new Observer object and register to Observable
     *
     * @param sub Observable to subscribe to
     */
    Observer(Observable<ObservableParamTypes...>& sub);
    /**
     * @brief Destroy the Observer object and unsubscribe from Observable
     *
     */
    ~Observer();

    // exposed functions
public:
    // interface
public:
    /**
     * @brief implement this to react to Observables.
     *
     * @param observable object beeing observed.
     * @param params
     */
    virtual void handle(Observable<ObservableParamTypes...>& observable,
                        ObservableParamTypes... params) = 0;

    // private variables
private:
    Observable<ObservableParamTypes...>& observable;

    // private functions
private:
    // static functions
private:
protected:
};
}  // namespace Patterns

#include "../src/Observer.cpp"

#endif  //__OBSERVER_H__