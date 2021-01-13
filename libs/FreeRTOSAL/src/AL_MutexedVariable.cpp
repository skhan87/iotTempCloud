/**
 * @file AL_MutexedVariable.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief variable that automaticly locks and unlocks for ipc.
 * @version 1.0
 * @date 2020-03-23
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_MutexedVariable.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

template<class T>
RTOS::MutexedVariable<T>::MutexedVariable(const T &&value) :
    mutex(), value(value)
{
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
template<class T>
Error::Code RTOS::MutexedVariable<T>::trySet(const T &          value,
                                      RTOS::milliseconds timeout)
{
    auto result = mutex.tryObtain(timeout);
    if (result != Error::None) {
        return result;
    }

    this->value = value;

    return mutex.tryRelease();
}

template<class T>
Error::Code RTOS::MutexedVariable<T>::trySetFromISR(const T &value,
                                             bool *   contextSwitchNeeded)
{
    auto result = mutex.tryObtainFromISR(contextSwitchNeeded);
    if (result != Error::None) {
        return result;
    }

    bool internalSwitchNeeded;
    this->value  = value;
    result = mutex.tryReleaseFromISR(&internalSwitchNeeded);
    *contextSwitchNeeded |= internalSwitchNeeded;
    return result;
}

template<class T>
Error::Code RTOS::MutexedVariable<T>::tryGet(T &value, RTOS::milliseconds timeout)
{
    auto result = mutex.tryObtain(timeout);
    if (result != Error::None) {
        return result;
    }

    value = this->value;
    return mutex.tryRelease();
}

template<class T>
Error::Code RTOS::MutexedVariable<T>::tryGetFromISR(T &   value,
                                             bool *contextSwitchNeeded)
{
    auto result = mutex.tryObtainFromISR(contextSwitchNeeded);
    if (result != Error::None) {
        return result;
    }

    value = this->value;

    bool internalSwitchNeeded;
    result = mutex.tryReleaseFromISR(&internalSwitchNeeded);
    *contextSwitchNeeded |= internalSwitchNeeded;
    return result;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------