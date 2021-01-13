/**
 * @file Observable.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief triggers observers, generic implementation of observer pattern
 * @version 1.0
 * @date 2019-10-26
 *
 * @copyright aconno GmbH (c) 2019
 *
 */

#ifndef __OBSERVABLE_H__
#define __OBSERVABLE_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace Patterns
{
template<class... ObservableParamTypes>
class Observable;
}

//--------------------------------- INCLUDES ----------------------------------

#include "Error.h"
#include "Observer.h"

#include <forward_list>

namespace Patterns
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Inherit this class, to supply an observable hook.
 *
 * @warning Does not implement inter process commmunication or conntext
 * switches. You can use RTOS synchronisation features in combination with this
 * to achieve that. Keep in mind, the Observable keeps a dynamic list
 * of Observers, so complexity is O(n) and also scales with Observer function
 * complexity.
 * 
 * @tparam ObservableParamTypes type of the parameter the event observer gets
 * called with.
 */
template<class... ObservableParamTypes>
class Observable {
    // structs and enums
public:
    // constructors
protected:
    /**
     * @brief creates the linked list for holding handlers.
     * It is heap alloc, to be able to dynamicly add and remove element.
     *
     */
    constexpr Observable() noexcept : observers() {};

    // exposed functions
public:
    /**
     * @brief registers a new observer that gets called, whenever this
     * Observable is triggered. Observer<> objects register themself.
     *
     * @param observer
     */
    void registerObserver(Observer<ObservableParamTypes...>& observer) noexcept;
    /**
     * @brief unregister given observer.
     *
     * @param observer
     */
    void unregisterObserver(
        Observer<ObservableParamTypes...>& observer) noexcept;

    // interface implementation
public:
    // private variables
private:
    // private functions
private:
    /**
     * @brief start point of linked list of all observers.
     *
     */
    std::forward_list<Observer<ObservableParamTypes...>*> observers;

    // static functions
private:
    // for subclasses
protected:
    /**
     * @brief trigger this Observable by calling all of its observers.
     *
     * @param args
     */
    void trigger(ObservableParamTypes...);
};
}  // namespace Patterns

// template classes need this
#include "../src/Observable.cpp"

#endif  //__OBSERVABLE_H__