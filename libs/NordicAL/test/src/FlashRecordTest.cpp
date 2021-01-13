/**
 * @file FlashRecordTest.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for flash writing and reading operations
 * @version 1.0
 * @date 2020-04-01
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "FlashRecordTest.h"

#include "TestEvent.h"

#include <cstddef>
#include <cstdint>

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::IOFlash::Record Test::IOFlash::Record::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::IOFlash::Record::Record()
        : Test::Base("IO::Flash", "Record"),
          file("RecordTest"), n1 {"Test", file}, n2 {"Awesome number", file}
{}

Test::IOFlash::Record& Test::IOFlash::Record::getInstance()
{
    return instance;
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief execution of test
 * 
 */
void Test::IOFlash::Record::runInternal()
{
    assert(Error::None == file.clear(), "failed to flush flash file");

    uint32_t v1 = 1234;
    int16_t  v2 = -321;

    assert(Error::None == n1.trySet(v1), "failed to set uint32_t to %u", v1);

    assert(Error::None == n2.trySet(v2), "failed to set int16_t to %d", v2);

    // modify to see, whether they get read properly
    ++v1;
    ++v2;

    auto result = n1.tryGet(v1);
    assert(Error::None == result,
           "failed to get uint32_t. Error Code: %d",
           result);
    result = n2.tryGet(v2);
    assert(Error::None == result,
           "failed to get int16_t. Error Code: %d",
           result);

    // try to rewrite an entry, one which can be
    // changed in place and one that has be rewritten
    assert(Error::None == n1.trySet(1234 | 32), "Failed to change n1 in place");
    assert(Error::None == n1.trySet(10), "Failed to rewrite n1 ");

    assert(v1 == 1234, "got wrong uint32_t value");
    assert(v2 == -321, "got wrong int16_t value");
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
