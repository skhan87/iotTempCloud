/**
 * @file TestLifetimeList.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief tests the LifetimeList class
 * @version 1.0
 * @date 2020-07-13
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestLifetimeList.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

/** eager loading singleton instance */
Test::LifetimeList Test::LifetimeList::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::LifetimeList::LifetimeList() : Base("Collections", "LifetimeList") {}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

void Test::LifetimeList::runInternal()
{
    // test list of values
    Collections::LifetimeList<int> listInt{};
    auto                           node1 = listInt.appendStatic(-1);
    auto                           node2 = listInt.appendDynamic(-2);
    testStatic(listInt);
    testDynamic(listInt);

    // test list of references
    Collections::LifetimeList<int&> listRef{};
    int                             x1    = 0;
    int                             x2    = 1;
    auto                            node3 = listRef.appendStatic(x1);
    auto                            node4 = listRef.appendDynamic(x2);
    assert(listRef.size() == 2, "reference LifetimeList failed");
    size_t cnt = 0;
    for (auto element : listRef) {
        // surpress unused
        (void)element;
        ++cnt;
    }
    assert(cnt == 2, "iterator over reference LifetimeList failed");

    // test list of pointers
    Collections::LifetimeList<int*> listPtr{};
    auto                            node5 = listPtr.appendStatic(&x1);
    auto                            node6 = listPtr.appendDynamic(&x2);
    assert(listPtr.size() == 2, "pointer LifetimeList failed");
    cnt = 0;
    for (auto element : listPtr) {
        // surpress unused
        (void)element;
        ++cnt;
    }
    assert(cnt == 2, "iterator over pointer LifetimeList failed");

    // test perfect forwarding
    Collections::LifetimeList<Dummy> listDummy{};
    // standard constructor
    auto node7 = listDummy.appendStatic();
    auto node8 = listDummy.appendDynamic();
    // copy constructor
    for (auto element : listDummy) {
        // surpress unused
        (void)element;
        auto node9 = listDummy.appendStatic(element);
    }
    // third constructor
    auto node10 =
        listDummy.pushStatic(static_cast<int>(10), static_cast<char>('l'));
    auto node11 =
        listDummy.pushDynamic(static_cast<int>(20), static_cast<char>('a'));
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief test for inserting static elements to an integer list
 * 
 * @param list 
 */
void Test::LifetimeList::testStatic(Collections::LifetimeList<int>& list)
{
    auto startCount = list.size();

    // add nodes
    auto node1 = list.pushStatic(1);
    auto node2 = list.appendStatic(2);
    auto node3 = list.pushStatic(3);
    auto node4 = list.appendStatic(4);

    assert(list.size() == (startCount + 4),
           "static adding of nodes went wrong");

    for (size_t i = 0; i < 10; i++) {
        auto innerNode1 = list.appendStatic(10);
        auto innerNode2 = list.pushStatic(11);
    }

    assert(list.size() == (startCount + 4),
           "static adding of nodes went wrong");

    size_t cnt = 0;
    for (auto element : list) {
        // surpress unused
        (void)element;
        ++cnt;
    }

    assert(cnt == (startCount + 4), "iterator does not properly work");
}

void Test::LifetimeList::testDynamic(Collections::LifetimeList<int>& list)
{
    auto startCount = list.size();

    // add nodes
    auto node1 = list.pushDynamic(1);
    auto node2 = list.appendDynamic(2);
    auto node3 = list.pushDynamic(3);
    auto node4 = list.appendDynamic(4);

    // release one node by hand
    node4.reset();

    assert(list.size() == (startCount + 3),
           "dynamic adding of nodes went wrong");

    for (size_t i = 0; i < 10; i++) {
        auto innerNode1 = list.appendDynamic(10);
        auto innerNode2 = list.pushDynamic(11);
    }

    assert(list.size() == (startCount + 3),
           "dynamic adding of nodes went wrong");

    size_t cnt = 0;
    for (auto element : list) {
        // surpress unused
        (void)element;
        ++cnt;
    }

    assert(cnt == (startCount + 3), "iterator does not properly work");
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief get singleton instance
 */
Test::LifetimeList& Test::LifetimeList::getInstance()
{
    return instance;
}
