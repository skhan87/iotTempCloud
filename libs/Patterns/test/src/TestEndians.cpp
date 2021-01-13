/**
 * @file TestEndians.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for endian conversion functions
 * @version 1.0
 * @date 2020-08-21
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestEndians.h"

#include <Endians.h>
#include <array>
#include <cstdint>

//--------------------------- STRUCTS AND ENUMS -------------------------------

/** singleton instance */
Test::Endians Test::Endians::instance{};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Endians::Endians() : Test::Base("Endians", "") {}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void Test::Endians::runInternal()
{
    uint8_t u8 = 1;
    ::Endians::machineToBig(u8);
    assert(u8 == 1, "uint8_t was changed by endian operations");

    uint16_t u16 = TEST_VAL_U16;
    ::Endians::machineToBig(u16);
    assert(reinterpret_cast<uint8_t*>(&u16)[1] == 1,
           "failed to convert uint16_t to big endian");
    ::Endians::bigToMachine(u16);
    assert(u16 == TEST_VAL_U16, "big endian operations failed on uint16_t");

    uint32_t u32 = TEST_VAL_U32;
    ::Endians::machineToLittle(u32);
    assert(reinterpret_cast<uint8_t*>(&u32)[0] == 1,
           "failed to convert uint32_t to little endian");
    ::Endians::littleToMachine(u32);
    assert(u32 == TEST_VAL_U32, "little endian operations failed on uint32_t");

    uint16_t a16[] = {TEST_VAL_U16, TEST_VAL_U16, TEST_VAL_U16};
    ::Endians::machineToBig(a16);
    for (const auto element : a16) {
        assert(reinterpret_cast<const uint8_t*>(&element)[1] == 1,
               "failed to convert uint16_t array to big endian");
    }

    std::array<uint32_t, 4> a32{TEST_VAL_U32,
                                TEST_VAL_U32,
                                TEST_VAL_U32,
                                TEST_VAL_U32};
    ::Endians::machineToLittle(a32);
    for (const auto element : a32) {
        assert(reinterpret_cast<const uint8_t*>(&element)[0] == 1,
               "failed to convert uint32_t array to little endian");
    }
}

Test::Endians& Test::Endians::getInstance()
{
    return instance;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------