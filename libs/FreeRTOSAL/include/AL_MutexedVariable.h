/**
 * @file AL_MutexedVariable.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief class to store a variable and lock and unlock automaticly for IPC
 * @version 1.0
 * @date 2020-03-23
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_MUTEXEDVARIABLE_H__
#define __AL_MUTEXEDVARIABLE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace RTOS
{
template<class T>
class MutexedVariable;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Mutex.h"
#include "FreeRTOSUtility.h"

namespace RTOS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief class to store a variable and lock and unlock automaticly for IPC
 *
 * @tparam T type of the variable to store
 */
template<class T>
class MutexedVariable
{
    // structs and enums
public:
    // constructors
public:
    // delete default constructors
    MutexedVariable()                             = delete;
    MutexedVariable(const MutexedVariable &other) = delete;
    MutexedVariable &operator=(const MutexedVariable &other) = delete;

    /**
     * @brief Construct a new Mutexed Variable with value
     *
     * @param other
     */
    MutexedVariable(const T &&value);

    // exposed functions
public:
    /**
     * @brief try to set the value
     *
     * @param value
     * @param timeoutMs
     * @return true success
     * @return Error::Timeout if mutex could not be obtained within timeoutMs
    */
    Error::Code trySet(const T &value, milliseconds timeoutMs = Infinity);
    /**
     * @brief tries to set the value from interrupt service routine.
     *
     * @param value
     * @param contextSwitchNeeded calling application needs to call scheduler.
     * @return Error::OutOfResources when mutex is already occupied
     */
    Error::Code trySetFromISR(const T &value, bool *contextSwitchNeeded = nullptr);
    /**
     * @brief try to get value
     *
     * @param value
     * @param timeoutMs
     * @return Error::Timeout if mutex could not be obtained within timeoutMs
     */
    Error::Code tryGet(T &value, milliseconds timeoutMs = Infinity);
    /**
     * @brief try to get the value from interrupt service routine
     *
     * @param value
     * @param contextSwitchNeeded whether the calling app needs to call
     * scheduler to allow for context switch
     * @return Error::OutOfResources when mutex is already occupied
     */
    Error::Code tryGetFromISR(T &value, bool *contextSwitchNeeded = nullptr);

    // interface implementation
public:
    // private variables
private:
    /** Mutex used to lock and unlock access to variable */
    Mutex mutex;
    /** variable to store the value in */
    T value;

    // private functions
private:
    // static functions
private:
protected:
};
}  // namespace RTOS

#include "../src/AL_MutexedVariable.cpp"
#endif  //__AL_MUTEXEDVARIABLE_H__