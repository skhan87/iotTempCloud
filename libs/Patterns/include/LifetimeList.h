/**
 * @file LifetimeList.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief generic list whichs nodes get managed by their lifetime
 * @version 1.0
 * @date 2020-07-10
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __LIFETIMELIST_H__
#define __LIFETIMELIST_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Collections
{
template <class T>
class LifetimeList;
}

//--------------------------------- INCLUDES ----------------------------------

#include <iterator>
#include <memory>

namespace Collections
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Linked list that add's and remove's with Node lifetime.
 * 
 * @warning do not use with stack allocated Nodes. They will be swapped out of scope.
 * 
 * New nodes are added with LifetimeList'a factory functions.
 * They are deleted when a Nodes destructor is called.
 */
template <class T>
class LifetimeList {
    // delete default constructors
    LifetimeList(const LifetimeList& other) = delete;
    LifetimeList& operator=(const LifetimeList& other) = delete;

public:
    /**
     * @brief Holds single values of the LifetimeList.
     * 
     * Entries in the LifetimeList are bound to the Lifetime of the Node.
     */
    class Node {
        // delete default constructors
        Node() = delete;

        friend LifetimeList<
            T>; /**< needs to be able to access private constructor */

    public:
        Node(const Node& other) = default;
        Node& operator=(const Node& other) = default;

        ~Node();

    private:
        template <class... ConstParamTypes>
        Node(LifetimeList<T>& list,
             Node*            insertAfter,
             ConstParamTypes&&... parameters);

        void insert(Node* insertAfter);

        T                value; /**< holds the value of the node */
        LifetimeList<T>& list;  /**< list this node belongs to */
        Node*
              previous; /**< previous element in the list. nullptr means first element */
        Node* next; /**< next element in the list. nullptr is last element */
    };

    /**
     * @brief iterates over a LifetimeList
     * 
     */
    class Iterator :
        public std::iterator<std::forward_iterator_tag,
                             T,
                             std::ptrdiff_t,
                             std::remove_reference_t<T>> {
        friend LifetimeList; /**< needed for private constructor */

    public:
        Iterator(const Iterator& other) = default;
        Iterator& operator=(const Iterator& other) = default;

    private:
        Iterator();
        Iterator(LifetimeList& lst);

    public:
        bool      operator==(const Iterator& other);
        bool      operator!=(const Iterator& other);
        T&        operator*() const;
        Iterator& operator++();

    private:
        Node* current; /**< node at the current position */
    };

    LifetimeList();
    ~LifetimeList();

    template <class... ConstParamTypes>
    Node pushStatic(ConstParamTypes&&... value);

    template <class... ConstParamTypes>
    Node appendStatic(ConstParamTypes&&... value);

    template <class... ConstParamTypes>
    std::unique_ptr<Node> pushDynamic(ConstParamTypes&&... value);

    template <class... ConstParamTypes>
    std::unique_ptr<Node> appendDynamic(ConstParamTypes&&... value);

    Iterator begin();
    Iterator end();
    size_t   size();

private:
    Node* root; /**< root(first) node of the collection */
    Node* tail; /**< tail(last) node of the collection */
};
}  // namespace Collections

// includes because this is a template class
#include "../src/LifetimeList.cpp"
#include "../src/LifetimeListIterator.cpp"
#include "../src/LifetimeListNode.cpp"
#endif  //__LIFETIMELIST_H__