/**
 * @file TestLifetimeList.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief tests the LifetimeList container class
 * @version 1.0
 * @date 2020-07-13
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTLIFETIMELIST_H__
#define __TESTLIFETIMELIST_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class LifetimeList;
}

//--------------------------------- INCLUDES ----------------------------------

#include "TestBase.h"

#include <LifetimeList.h>

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief tests the LifetimeList container class
 */
class LifetimeList : Base {
    // delete default constructors
    LifetimeList(const LifetimeList& other) = delete;
    LifetimeList& operator=(const LifetimeList& other) = delete;

public:
    virtual void runInternal() final;
    static LifetimeList& getInstance();

private:
    LifetimeList();
    static LifetimeList instance;

    /**
     * @brief class to test perfect forwarding
     * 
     */
    class Dummy {
    public:
        Dummy() {}
        Dummy(int, char) {}
        Dummy(const Dummy&) {}
    };

    void testStatic(Collections::LifetimeList<int>& list);
    void testDynamic(Collections::LifetimeList<int>& list);
};
}  // namespace Test
#endif  //__TESTLIFETIMELIST_H__