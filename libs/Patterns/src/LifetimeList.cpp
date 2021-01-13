/**
 * @file LifetimeList.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief generic list of nodes that get added and removed by their lifetime.
 * @version 1.0
 * @date 2020-07-10
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "LifetimeList.h"

#include <Error.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new, empty Lifetime List
 * 
 */
template <class T>
Collections::LifetimeList<T>::LifetimeList() : root(nullptr), tail(nullptr)
{}

/**
 * @brief Does sanity checks. Non empty lists must not be destroyed!
 * All contained nodes have to be destroyed before the list is.
 * 
 * @warning Non empty lists that get destroyed will cause an error.
 * 
 * @tparam T 
 */
template <class T>
Collections::LifetimeList<T>::~LifetimeList()
{
    if (size() != 0) {
        CHECK_ERROR(Error::Lifetime);
    }
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief factory function to add a node at the start of the list
 * 
 * @warning the added Node only persists until it runs out of scope.
 * In that case it removes itself from the list.
 * 
 * @tparam T type of elements in container
 * @tparam ConstParamTypes Type of parameters to initialize T object enclosed by Node
 * @param parameters parameters to initialize T object enclosed by Node
 * @return Collections::LifetimeList<T>::Node new node in the list that contains given object
 */
template <class T>
template <class... ConstParamTypes>
typename Collections::LifetimeList<T>::Node Collections::LifetimeList<
    T>::pushStatic(ConstParamTypes&&... parameters)
{
    return Node(*this,
                static_cast<Node*>(nullptr),
                std::forward<ConstParamTypes>(parameters)...);
}

/**
 * @brief factory function to add a node at the end of the list
 * 
 * @warning the added Node only persists until it runs out of scope.
 * In that case it removes itself from the list.
 * 
 * @tparam T type of elements in container
 * @tparam ConstParamTypes Type of parameters to initialize T object enclosed by Node
 * @param parameters parameters to initialize T object enclosed by Node
 * @return Collections::LifetimeList<T>::Node new node in the list that contains given object
 */
template <class T>
template <class... ConstParamTypes>
typename Collections::LifetimeList<T>::Node Collections::LifetimeList<
    T>::appendStatic(ConstParamTypes&&... parameters)
{
    return Node(*this, tail, std::forward<ConstParamTypes>(parameters)...);
}

/**
 * @brief factory function to add a node at the start of the list
 * 
 * @warning the added Node is heap allocated. Only use when dynamic object lifetime is needed.
 * 
 * @tparam T type of elements in container
 * @tparam ConstParamTypes Type of parameters to initialize T object enclosed by Node
 * @param parameters parameters to initialize T object enclosed by Node
 * @return Collections::LifetimeList<T>::Node new node in the list that contains given object
 */
template <class T>
template <class... ConstParamTypes>
std::unique_ptr<typename Collections::LifetimeList<T>::Node> Collections::
    LifetimeList<T>::pushDynamic(ConstParamTypes&&... parameters)
{
    return std::unique_ptr<Node>(
        new Node(*this,
                 static_cast<Node*>(nullptr),
                 std::forward<ConstParamTypes>(parameters)...));
}

/**
 * @brief @brief factory function to add a node at the end of the list
 * 
 * @warning the added Node is heap allocated. Only use when dynamic object lifetime is needed.
 * 
 * @tparam T type of elements in container
 * @tparam ConstParamTypes Type of parameters to initialize T object enclosed by Node
 * @param parameters parameters to initialize T object enclosed by Node
 * @return Collections::LifetimeList<T>::Node new node in the list that contains given object
 */
template <class T>
template <class... ConstParamTypes>
std::unique_ptr<typename Collections::LifetimeList<T>::Node> Collections::
    LifetimeList<T>::appendDynamic(ConstParamTypes&&... parameters)
{
    return std::unique_ptr<Node>(
        new Node(*this,
                 static_cast<Node*>(tail),
                 std::forward<ConstParamTypes>(parameters)...));
}

/**
 * @brief get begin iterator 
 * 
 * @return Iterator 
 */
template <class T>
typename Collections::LifetimeList<T>::Iterator Collections::LifetimeList<
    T>::begin()
{
    return Iterator(static_cast<LifetimeList&>(*this));
}

/**
 * @brief get end iterator
 * 
 * @return Iterator 
 */
template <class T>
typename Collections::LifetimeList<T>::Iterator Collections::LifetimeList<
    T>::end()
{
    return Iterator();
}

/**
 * @brief returns the number of elements in this LifetimeList
 * 
 * @tparam T 
 * @return size_t 
 */
template <class T>
size_t Collections::LifetimeList<T>::size()
{
    size_t n    = 0;
    auto   node = begin();
    for (; node != end(); ++node) {
        ++n;
    }
    return n;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------