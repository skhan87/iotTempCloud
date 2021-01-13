/**
 * @file ParsedAdvData.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Parsed advertisement data
 * @version 1.0
 * @date 2020-11-06
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __PARSEDADVDATA_H__
#define __PARSEDADVDATA_H__

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Device.h"

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Parsed advertisement data package.
 * 
 * @warning Most member are pointers and therefor
 * nullable. If they did not show up in the 
 * advertisement package, they will be nullptr in
 * this struct.
 */
struct ParsedAdvData {
    // delete default constructors
    ParsedAdvData(const ParsedAdvData& other) = delete;
    ParsedAdvData& operator=(const ParsedAdvData& other) = delete;

    ParsedAdvData()        = default;
    ParsedAdvData& operator=(ParsedAdvData&& other) = default;

    std::unique_ptr<uint8_t[]> manufacturerData;
    size_t                     manufacturerDataSize;
    std::unique_ptr<char[]>    name;
    size_t                     nameSize;
    Flags                      flags;
    Appearance                 appearance;

    static Error::Code parseRawData(ParsedAdvData&             outData,
                                    std::unique_ptr<uint8_t[]> rawData,
                                    const size_t               rawDataSize);
};
}  // namespace IO::BLE
#endif  //__PARSEDADVDATA_H__