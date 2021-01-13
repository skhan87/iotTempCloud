/**
 * @file Bitfield.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Portable library for Bitfield operations
 * @version 1.0
 * @date 2020-09-30
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __BITFIELD_H__
#define __BITFIELD_H__

//--------------------------------- INCLUDES ----------------------------------

#include <Endians.h>
#include <Error.h>
#include <array>

namespace Endians
{
/**
 * @brief anonymous namespace to hide internal workings of this lib.
 * 
 */
namespace
{
/**
     * @brief Copies the source bit from source byte to destination bit in destination byte
     * 
     * @param dest Reference to destination byte to write to
     * @param destPosition Bit in destination byte to write to
     * @param src Source byte to read from
     * @param srcPosition Bit in SourceByte to read from
     */
void copyBit(uint8_t& dest, size_t destPosition, uint8_t src, size_t srcPosition)
{
    if ((src & (1u << srcPosition)) != 0) {
        // bit set in src, set in dest
        dest |= (1u << destPosition);
    } else {
        // bit not set in src, unset in dest
        dest &= ~(1u << destPosition);
    }
}
}  // namespace

/**
 * @brief Order of bits in bytes.
 * 
 * @details Standard BitOrder on native is always LSBAtZero
 * since static_cast<unsigned int>(1<<0) always results in
 * a 1.
 * 
 * @warning This is not Endianess!
 * 
 */
enum class BitOrder {
    /**
         * @brief The least significant bit is masked by (1u << 0).
         * 
         * @details This implies MSB is masked by (1u << 7).
         * This is the normal machine bitorder on all machines.
         * 
         */
    LSBAtZero,
    /**
         * @brief The most significant bit is masked by (1u << 0).
         * 
         * @details This implies LSB is masked by (1u << 7).
         * This is not the normal machine bitorder. For the machine
         * to be able to work with this, the bits have to be swapped.
         * 
         */
    MSBAtZero
};

/**
 * @brief Template class to hold information about a single bitfield.
 * 
 * @warning Can only be used with arithmetic types. The last enable_if_t implements
 * a protection against using anything else with this functions.
 * When tried with another type, will result in compiler error similar to
 * "no type named ‘type’".
 * 
 * @example Simple usage to read and write an unsigned int:
 * #include <Bitfield.h>
 * 
 * using MostSignificantBit = Endians::Bitfield<unsigned int, 7, 1>;
 * uint8_t data1 = 0b1000000;
 * auto value = MostSignificantBit::get(data1); // will return 1
 * CHECK_ERROR(MostSignificantBit::set(data1, false)); // will make data1 == 0u
 * 
 * Reading more from data arrays:
 * 
 * std::array<uint8_t, 3> data2 = {0b00000001, 0b10000000, 0b00000100};
 * value = MostSignificantBit::get(data2); // will return false
 * value = MostSignificantBit::get<3, 1>(data2); // gets MSB from second byte -> 1
 * value = MostSignificantBit::get<3, 2>(data2); // gets MSB from third byte -> 0
 * 
 * For further examples look in "TestBitfield.h".
 * 
 * @details Also supplies additional parameters to handle inversed BitOrder and Endianess.
 * Peripheral refers to communications partner like a sensor on a bus or a
 * connnection BLE device.
 * All functions are for getting from and setting to a uint8_t array. This is for
 * machine independence and to ensure proper byte order of incomming streams.
 * 
 * @tparam T Type of the value in the bitfield. Must be arithmetic.
 * @tparam position 0-indexed position in bits.
 * @tparam size Size of the bitfield in bits.
 * @tparam fieldOrder BitOrder of the Bitfield itself. Is used front to back for the whole field.
 * @tparam peripheralOrder BitOrder of the data bytearray received from and send to peripheral. Bitorder WITHIN each byte. Not the whole byte array.
 * @tparam endianess Endianess of the field as it is when comming from the peripheral. Applied after bitorder operations on getting and befor on setting.
 */
template<class T,
         size_t    position,
         size_t    size,
         BitOrder  fieldOrder           = BitOrder::LSBAtZero,
         BitOrder  peripheralOrder      = BitOrder::LSBAtZero,
         ByteOrder endianess            = ByteOrder::little,
         typename std::enable_if_t<std::is_arithmetic<T>::value ||
                                       std::is_enum<T>::value,
                                   int> = 0>
struct Bitfield {
    /**
     * @brief Get the value of a bitfield out of a byte array
     * 
     * @tparam arraySize size of the data array
     * @tparam byteOffset=0 offset in bytes to read the Bitfield from. (So it can also be read out of a stream or bigger array).
     * @param data data array received from peripheral. Must be at least arraySize long.
     * @return T Value of the Bitfield in data.
     */
    template<size_t arraySize, size_t byteOffset = 0>
    static T get(const uint8_t* data)
    {
        static_assert(position + size + (8 * byteOffset) <= 8 * arraySize,
                      "Bitfield getter out of boundries");
        static_assert(sizeof(T) * 8 >= size,
                      "Bitfield size too big for chosen data type");

        T              result {};
        auto           ref         = reinterpret_cast<uint8_t*>(&result);
        constexpr auto startBitSrc = position + (8 * byteOffset);
        constexpr auto endBitSrc   = startBitSrc + size;
        for (size_t desBit = 0; desBit < size; ++desBit) {
            size_t srcBit;
            // srcBit counts left to right when fieldOrder is inversed
            if (fieldOrder == BitOrder::LSBAtZero) {
                srcBit = desBit + startBitSrc;
            } else {
                srcBit = endBitSrc - desBit - 1;
            }

            auto srcByte = srcBit / 8;
            srcBit %= 8;

            // read the other way around from data array if peripheral order is inversed
            if (peripheralOrder == BitOrder::MSBAtZero) {
                srcBit = 7 - srcBit;
            }
            copyBit(ref[desBit / 8], desBit % 8, data[srcByte], srcBit);
        }

        // compensate for 2's complement
        if (std::is_signed<T>::value &&
            (ref[(size - 1) / 8] & (1u << ((size - 1) % 8)))) {
            // highest bit is set, inverse all bits above it
            for (size_t desBit = size; desBit < (8 * sizeof(T)); ++desBit) {
                ref[desBit / 8] |= (1u << (desBit % 8));
            }
        }

        if (endianess == Endians::ByteOrder::big) {
            Endians::bigToMachine(result);
        } else {
            Endians::littleToMachine(result);
        }

        return result;
    }

    /**
     * @brief Set the bitfield value within the data array.
     * 
     * @tparam arraySize Size of the data array.
     * @tparam byteOffset=0 Offset in data to write to. In Bytes.
     * @param data Data array to write to. Must be at least arraySize long.
     * @param value Value to write to the bitfield value.
     * 
     * @return Error::Code Can return Error::TooLarge when biggest bit of value is higher than size of the Bitfield.
     */
    template<size_t arraySize, size_t byteOffset = 0>
    static Error::Code set(uint8_t* data, T value)
    {
        static_assert(position + size + (8 * byteOffset) <= 8 * arraySize,
                      "Bitfield getter out of boundries");
        static_assert(sizeof(T) * 8 >= size,
                      "Bitfield size too big for chosen data type");

        if (endianess == Endians::ByteOrder::big) {
            Endians::machineToBig(value);
        } else {
            Endians::machineToLittle(value);
        }
        auto ref = reinterpret_cast<const uint8_t*>(&value);

        // copy bitwise
        constexpr auto startBitDes = position + (8 * byteOffset);
        constexpr auto endBitDes   = startBitDes + size;
        for (size_t srcBit = 0; srcBit < size; ++srcBit) {
            size_t desBit;
            // srcBit counts left to right when fieldOrder is inversed
            if (fieldOrder == BitOrder::LSBAtZero) {
                desBit = srcBit + startBitDes;
            } else {
                desBit = endBitDes - srcBit - 1;
            }

            auto desByte = desBit / 8;
            desBit %= 8;

            if (peripheralOrder == BitOrder::MSBAtZero) {
                desBit = 7 - desBit;
            }

            copyBit(data[desByte], desBit, ref[srcBit / 8], srcBit % 8);
        }

        return Error::None;
    }

    // specializations for more restricted cases

    /**
     * @brief Get the value of the Bitfield out of one byte
     * 
     * @param data data byte containing the Bitfield
     * @return T value of the Bitfield in given data
     */
    static T get(uint8_t data) { return get<1, 0>(&data); }

    /**
     * @brief Get the value of a bitfield out of a byte array
     * 
     * @tparam arraySize size of the data array
     * @tparam byteOffset=0 offset in bytes to read the Bitfield from. (So it can also be read out of a stream or bigger array).
     * @param data data array received from peripheral. Must be at least arraySize long.
     * @return T Value of the Bitfield in data.
     */
    template<size_t arraySize, size_t byteOffset = 0>
    static T get(const std::array<uint8_t, arraySize>& data)
    {
        return get<arraySize, byteOffset>(data.data());
    }

    /**
     * @brief Set the Bits described by Bitfield in given data byte.
     * 
     * @param data Data to write the Bitfield value to.
     * @param value Value to generate Bits from.
     * 
     * @return Error::Code Can return Error::TooLarge when biggest bit of value is higher than size of the Bitfield.
     */
    static Error::Code set(uint8_t& data, T value)
    {
        return set<1, 0>(&data, value);
    }

    /**
     * @brief Set the bitfield value within the data array.
     * 
     * @tparam arraySize Size of the data array.
     * @tparam byteOffset=0 Offset in data to write to. In Bytes.
     * @param data Data array to write to. Must be at least arraySize long.
     * @param value Value to write to the bitfield value.
     * 
     * @return Error::Code Can return Error::TooLarge when biggest bit of value is higher than size of the Bitfield.
     */
    template<size_t arraySize, size_t byteOffset = 0>
    static Error::Code set(std::array<uint8_t, arraySize>& data, T value)
    {
        return set<arraySize, byteOffset>(data.data(), value);
    }
};
}  // namespace Endians
#endif  //__BITFIELD_H__