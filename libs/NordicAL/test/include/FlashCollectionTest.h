/**
 * @file FlashCollectionTest.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief tests the IOFlash::Collection class
 * @version 1.0
 * @date 2020-08-11
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __FLASHCOLLECTIONTEST_H__
#define __FLASHCOLLECTIONTEST_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test::IOFlash
{
class Collection;
}

//--------------------------------- INCLUDES ----------------------------------

#include <AL_FlashCollection.h>
#include <TestBase.h>

namespace Test::IOFlash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief tests the IOFlash::Collection class
 */
class Collection : public Test::Base {
    // delete default constructors
    Collection(const Collection& other) = delete;
    Collection& operator=(const Collection& other) = delete;

public:
    static Collection& getInstance();

private:
    Collection();

    virtual void runInternal() final;

    struct TestStruct {
        uint32_t a;
        uint64_t b;
        uint32_t c[16] = {0};

        bool operator==(const TestStruct& other)
        {
            return a == other.a && b == other.b &&
                   std::equal(std::begin(c), std::end(c), std::begin(other.c));
        }
    };

    IO::Flash::Collection<int>        collectionInt;
    IO::Flash::Collection<TestStruct> collectionStruct;

    static Collection instance;
};
}  // namespace Test::IOFlash
#endif  //__FLASHCOLLECTIONTEST_H__