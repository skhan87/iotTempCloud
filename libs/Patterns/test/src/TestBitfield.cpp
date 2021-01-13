/**
 * @file TestBitfield.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Test for Bitfield manipulation lib
 * @version 1.0
 * @date 2020-10-01
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "TestBitfield.h"

#include "TestEndians.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

Test::Bitfield Test::Bitfield::instance {};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

Test::Bitfield::Bitfield() : Test::Base("Endians", "Bitfield") {}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void Test::Bitfield::runInternal()
{
    assert(LSB::get(kData1) == true, "LSB standard reading failed");
    assert(MSB::get(kData1) == false, "MSB standard reading failed");
    assert(LSBInversePeripheral::get(kData1) == false,
           "LSB inverse reading failed");
    assert(MSBInversePeripheral::get(kData1) == true,
           "MSB inverse reading failed");

    assert(LSB::get<3, 1>(kData2) == true,
           "LSB standard reading on array failed");
    assert(MSB::get<3, 1>(kData2) == false,
           "MSB standard reading on array failed");

    assert(InterByteLsbLsb::get(kData2) == 6u,
           "Failed to read InterByteLsbLsb");
    assert(InterByteLsbMsb::get(kData2) == 1u,
           "Failed to read InterByteLsbMsb");
    assert(InterByteMsbLsb::get(kData2) == 3u,
           "Failed to read InterByteMsbLsb");
    assert(InterByteMsbMsb::get(kData2) == 4u,
           "Failed to read InterByteMsbMsb");

    assert(BigEndianUint16::get(kData2) == 17667u,
           "Failed to read BigEndianUint16");
    assert(LittleEndianUint16::get(kData2) == 837u,
           "failed to read LittleEndianUint16");

    assert(SignedInt::get(kData1) == -3, "SignedInt reading failed");

    uint8_t data1   = 0;
    auto    errCode = LSB::set(data1, true);
    assert((errCode == Error::None) && (data1 == 0b00000001),
           "LSB setter failed: %x",
           data1);

    data1   = 0;
    errCode = MSB::set(data1, true);
    assert((errCode == Error::None) && (data1 == 0b10000000),
           "MSB setter failed: %x",
           data1);

    data1   = 0;
    errCode = LSBInversePeripheral::set(data1, true);
    assert((errCode == Error::None) && (data1 == 0b10000000),
           "LSB inverse setter failed: %x",
           data1);

    data1   = 0;
    errCode = MSBInversePeripheral::set(data1, true);
    assert((errCode == Error::None) && (data1 == 0b00000001),
           "MSB inverse setter failed: %x",
           data1);

    std::array<uint8_t, 3> data2 = {0, 0, 0};
    errCode                      = LSB::set<3, 1>(data2, true);
    assert((errCode == Error::None) &&
               ((data2[0] == 0) && (data2[1] == 0b00000001) && (data2[2] == 0)),
           "LSB setter failed on array");

    data2   = {0, 0, 0};
    errCode = MSB::set<3, 1>(data2, true);
    assert((errCode == Error::None) &&
               ((data2[0] == 0) && (data2[1] == 0b10000000) && (data2[2] == 0)),
           "MSB setter failed on array");

    data2   = {0, 0, 0};
    errCode = InterByteLsbLsb::set(data2, 0b110);
    assert((errCode == Error::None) &&
               ((data2[0] == 0) && (data2[1] == 0b00000011) && (data2[2] == 0)),
           "InterbyteLsbLsb setter failed");

    data2   = {0, 0, 0};
    errCode = InterByteLsbMsb::set(data2, 0b110);
    assert((errCode == Error::None) &&
               ((data2[0] == 0b00000000) && (data2[1] == 0b11000000) &&
                (data2[2] == 0)),
           "InterbyteLsbMsb setter failed");

    data2   = {0, 0, 0};
    errCode = InterByteMsbLsb::set(data2, 0b110);
    assert((errCode == Error::None) &&
               ((data2[0] == 0b10000000) && (data2[1] == 0b00000001) &&
                (data2[2] == 0)),
           "InterbyteMsbLsb setter failed");

    data2   = {0, 0, 0};
    errCode = InterByteMsbMsb::set(data2, 0b110);
    assert((errCode == Error::None) &&
               ((data2[0] == 0b00000001) && (data2[1] == 0b10000000) &&
                (data2[2] == 0)),
           "InterbyteMsbMsb setter failed");

    data2   = {0, 0, 0};
    errCode = BigEndianUint16::set(data2, 384u);
    assert((errCode == Error::None) &&
               ((data2[0] == 0b00000001) && (data2[1] == 0b10000000) &&
                (data2[2] == 0)),
           "BigEndianUint16 setter failed");

    data2   = {0, 0, 0};
    errCode = LittleEndianUint16::set(data2, 384u);
    assert((errCode == Error::None) &&
               ((data2[0] == 0b10000000) && (data2[1] == 0b00000001) &&
                (data2[2] == 0)),
           "LittleEndianUint16 setter failed");

    uint8_t data3 = 0;
    errCode       = SignedInt::set(data3, -2);
    assert(data3 == 0b00000110, "SignedInt set failed");
}

const std::list<Test::Base*> Test::Bitfield::getPrerequisits()
{
    return std::list<Test::Base*>({&Test::Endians::getInstance()});
}

Test::Bitfield& Test::Bitfield::getInstance()
{
    return instance;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------