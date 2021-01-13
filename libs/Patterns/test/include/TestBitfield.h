/**
 * @file TestBitfield.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for the bitfield library
 * @version 1.0
 * @date 2020-10-01
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __TESTBIEFIELD_H__
#define __TESTBIEFIELD_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test
{
class Bitfield;
}

//--------------------------------- INCLUDES ----------------------------------

#include <Bitfield.h>
#include <TestBase.h>
#include <array>

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for the bitfield library
 */
class Bitfield : public Base {
    // delete default constructors
    Bitfield(const Bitfield& other) = delete;
    Bitfield& operator=(const Bitfield& other) = delete;

public:
    virtual void                   runInternal() final;
    virtual const std::list<Base*> getPrerequisits() final;
    static Bitfield&               getInstance();

private:
    Bitfield();
    static Bitfield instance;

    using LSB = Endians::Bitfield<bool, 0, 1>;
    using MSB = Endians::Bitfield<bool, 7, 1>;
    using LSBInversePeripheral =
        Endians::Bitfield<bool,
                          0,
                          1,
                          Endians::BitOrder::LSBAtZero,
                          Endians::BitOrder::MSBAtZero>;
    using MSBInversePeripheral =
        Endians::Bitfield<bool,
                          7,
                          1,
                          Endians::BitOrder::LSBAtZero,
                          Endians::BitOrder::MSBAtZero>;
    using InterByteLsbLsb = Endians::Bitfield<uint8_t, 7, 3>;
    using InterByteLsbMsb = Endians::Bitfield<uint8_t,
                                              7,
                                              3,
                                              Endians::BitOrder::LSBAtZero,
                                              Endians::BitOrder::MSBAtZero>;
    using InterByteMsbLsb = Endians::Bitfield<uint8_t,
                                              7,
                                              3,
                                              Endians::BitOrder::MSBAtZero,
                                              Endians::BitOrder::LSBAtZero>;
    using InterByteMsbMsb = Endians::Bitfield<uint8_t,
                                              7,
                                              3,
                                              Endians::BitOrder::MSBAtZero,
                                              Endians::BitOrder::MSBAtZero>;

    using BigEndianUint16    = Endians::Bitfield<uint16_t,
                                              0,
                                              16,
                                              Endians::BitOrder::LSBAtZero,
                                              Endians::BitOrder::LSBAtZero,
                                              Endians::ByteOrder::big>;
    using LittleEndianUint16 = Endians::Bitfield<uint16_t,
                                                 0,
                                                 16,
                                                 Endians::BitOrder::LSBAtZero,
                                                 Endians::BitOrder::LSBAtZero,
                                                 Endians::ByteOrder::little>;
    using SignedInt          = Endians::Bitfield<signed int, 0, 3>;

    static constexpr uint8_t                kData1 = 0b01000101;
    static constexpr std::array<uint8_t, 3> kData2 = {0b01000101,
                                                      0b00000011,
                                                      0b00000010};
};
}  // namespace Test
#endif  //__TESTBIEFIELD_H__