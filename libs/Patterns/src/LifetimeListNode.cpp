/**
 * @file LifetimeListNode.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief implementation of nodes for the LifetimeList class
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
 * @brief Makes a new Node in a LifetimeList.
 * 
 * @tparam T type of the objects listed
 * @tparam ConstParamTypes type of parameters to initialize the encapsulated object with
 * @param list list to add this node to
 * @param insertAfter node in the same list to insert this after
 * @param parameters parameters to initialize the encapsulated object with
 */
template <class T>
template <class... ConstParamTypes>
Collections::LifetimeList<T>::Node::Node(LifetimeList<T>& list,
                                         Node*            insertAfter,
                                         ConstParamTypes&&... parameters)
    : value(std::forward<ConstParamTypes>(parameters)...), list(list),
      previous(nullptr), next(nullptr)
{
    insert(insertAfter);
}

/**
 * @brief inserts this after given node into the LinkedList
 * 
 * @tparam T type of encapsulated objects
 * @param insertAfter node in the same list to insert this
 */
template <class T>
void Collections::LifetimeList<T>::Node::insert(Node* insertAfter)
{
    if ((insertAfter != nullptr) && (&insertAfter->list != &list)) {
        // sanity check: insertAfter does not belong to the same list
        CHECK_ERROR(Error::InvalidParameter);
    }

    if (list.root == nullptr) {
        // empty list
        if (insertAfter != nullptr) {
            // sanity check: insert after must be nullptr for empty list
            CHECK_ERROR(Error::InvalidParameter);
        }

        list.root = this;
        list.tail = this;
        return;
    }

    if (insertAfter == nullptr) {
        // insert as first element
        next                = list.root;
        list.root->previous = this;
        list.root           = this;
    } else {
        // insert after node
        if (insertAfter == list.tail) {
            list.tail = this;
        }
        next = insertAfter->next;
        if (next != nullptr) {
            next->previous = this;
        }
        previous          = insertAfter;
        insertAfter->next = this;
    }
}

/**
 * @brief Remove node from LifetimeList
 * 
 */
template <class T>
Collections::LifetimeList<T>::Node::~Node()
{
    if (previous != nullptr) {
        previous->next = next;
    } else {
        // first element
        list.root = next;
    }

    if (next != nullptr) {
        next->previous = previous;
    } else {
        // last element
        list.tail = previous;
    }
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------