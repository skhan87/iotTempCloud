/**
 * @file FlashCollectionTest.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test of the flash collection
 * @version 1.0
 * @date 2020-08-11
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "FlashCollectionTest.h"

#include "AL_Log.h"
#include "TestEvent.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

/** singleton instance */
Test::IOFlash::Collection Test::IOFlash::Collection::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::IOFlash::Collection::Collection()
        : Base("IO::Flash", "Collection"),
          collectionInt("testIntegers"), collectionStruct {"testStructs"}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief get singleton instance.
 * 
 * @return Test::IOFlash::Collection& 
 */
Test::IOFlash::Collection& Test::IOFlash::Collection::getInstance()
{
    return instance;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief execution of test
 * 
 */
void Test::IOFlash::Collection::runInternal()
{
    assert(Error::None == collectionInt.clear(), "failed to flush collection");

    assert(Error::None == collectionInt.add(1), "Failed to add element");
    assert(Error::None == collectionInt.add(2), "failed to add element");
    assert(Error::None == collectionInt.remove(1), "failed to remove");
    assert(Error::None == collectionInt.add(3), "failed to add element");

    assert(collectionInt.size() == 2, "wrong collection size");

    assert(Error::None == collectionInt.clear(), "failed to flush collection");

    assert(Error::None == collectionStruct.clear(),
           "failed to flush collection");

    assert(Error::None == collectionStruct.add({1, 1}),
           "Failed to add element");
    assert(Error::None == collectionStruct.add({2, 2}),
           "failed to add element");
    assert(Error::None == collectionStruct.remove({1, 1}),
           "failed to remove element");
    assert(Error::None == collectionStruct.add({3, 3}),
           "failed to add element");

    assert(collectionStruct.size() == 2, "wrong collection size");

    assert(Error::None == collectionStruct.clear(),
           "failed to flush collection");

    // fill flash up
    bool full = false;
    while (!full) {
        auto errCode = collectionStruct.add({1, 1});
        switch (errCode) {
            case Error::None: {
                // dummy assert to count succesful adds
                assert(true, "Unknown");
                break;
            }
            case Error::OutOfResources: {
                // flash is full now, delete up to 100 entries
                full = true;
                for (size_t i = 0; i < 100; ++i) {
                    auto iter = collectionStruct.begin();
                    if (iter == collectionStruct.end()) {
                        // end of collection
                        break;
                    }
                    auto value = *iter;

                    assert(collectionStruct.remove(value) == Error::None,
                           "Failed to remove entries from full flash "
                           "collection");
                }
            }
            default: {
                // some other error, assert
                assert(errCode, "Error while trying to fill up collection");
                full = true;
                break;
            }
        }
    }

    // add another 10 to ensure garbage collection is working
    for (int i = 0; i < 10; ++i) {
        assert(collectionStruct.add({1, 1}) == Error::None,
               "failed to add after garbage collection should have cleaned "
               "flash");
    }
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------