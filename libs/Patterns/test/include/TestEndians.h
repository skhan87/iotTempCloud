/**
 * @file TestEndians.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for the endian conversion functions
 * @version 1.0
 * @date 2020-08-21
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTENDIANS_H__
#define __TESTENDIANS_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Endians;
}

//--------------------------------- INCLUDES ----------------------------------

#include "TestBase.h"

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for the endian conversion functions
 */
class Endians : public Test::Base {
    // delete default constructors
    Endians(const Endians& other) = delete;
    Endians& operator=(const Endians& other) = delete;

public:
    virtual void    runInternal() final;
    static Endians& getInstance();

private:
    Endians();
    static Endians instance;

    static constexpr uint16_t TEST_VAL_U16 = (2 << 8) + 1;
    static constexpr uint32_t TEST_VAL_U32 =
        (4 << 24) + (3 << 16) + (2 << 8) + 1;
};
}  // namespace Test
#endif  //__TESTENDIANS_H__