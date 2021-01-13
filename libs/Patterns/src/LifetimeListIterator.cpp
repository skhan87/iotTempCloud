/**
 * @file LifetimeListIterator.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief iterator for LifetimeList
 * @version 1.0
 * @date 2020-07-10
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "LifetimeList.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief creates an empty iterator
 * 
 * @tparam T datatype of objects in LifetimeList
 */
template <class T>
Collections::LifetimeList<T>::Iterator::Iterator() : current(nullptr)
{}

/**
 * @brief creates an iterator over lst
 * 
 * @tparam T 
 * @param lst 
 * @param atEnd false: start in front, true: start at end
 */
template <class T>
Collections::LifetimeList<T>::Iterator::Iterator(LifetimeList<T>& lst)
    : current(lst.root)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief comperator to fulfill the iterator interface
 * 
 * @tparam T 
 * @param other 
 * @return true 
 * @return false 
 */
template <class T>
bool Collections::LifetimeList<T>::Iterator::operator==(const Iterator& other)
{
    return other.current == current;
}

/**
 * @brief comperator to fulfill the iterator interface
 * 
 * @tparam T 
 * @param other 
 * @return true 
 * @return false 
 */
template <class T>
bool Collections::LifetimeList<T>::Iterator::operator!=(const Iterator& other)
{
    return other.current != current;
}

/**
 * @brief returns the encapsuled value of current node
 * 
 * @tparam T 
 * @return T 
 */
template <class T>
T& Collections::LifetimeList<T>::Iterator::operator*() const
{
    return current->value;
}

/**
 * @brief iterate over the linked list
 * 
 * @tparam T 
 * @return Collections::LifetimeList<T>::Iterator& 
 */
template <class T>
typename Collections::LifetimeList<T>::Iterator& Collections::LifetimeList<
    T>::Iterator::operator++()
{
    if (current != nullptr) {
        current = current->next;
    }
    return *this;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------