/**
 * @file ParsedAdvData.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Parsed advertisement data package.
 * @version 1.0
 * @date 2020-11-06
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "Endians.h"
#include "ParsedAdvData.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Takes raw adv data and parses it
 * 
 * @param outData Parsed data output.
 * @param rawData Pointer to raw data in Heap.
 * @param rawDataSize Size of the raw data.
 * @return Error::Code Might fail on length checks for certain fields.
 */
Error::Code
    IO::BLE::ParsedAdvData::parseRawData(IO::BLE::ParsedAdvData&    outData,
                                         std::unique_ptr<uint8_t[]> rawData,
                                         const size_t               rawDataSize)
{
    outData     = std::move(ParsedAdvData {});
    auto result = Error::None;

    size_t offset    = 0;
    size_t fieldSize = 0;

    // Disassemble advertisement field by field
    while (offset < rawDataSize) {
        fieldSize =
            rawData[offset++] - 1;  // fieldSize also contains fieldType size 1
        AdvDataField fieldType = static_cast<AdvDataField>(rawData[offset++]);

        switch (fieldType) {
            case AdvDataField::Flags: {
                if (fieldSize != (1)) {
                    // do not use sizeof(Flags) might be different than 1
                    return Error::SizeMissmatch;
                }
                outData.flags = static_cast<Flags>(rawData[offset]);
                break;
            }

            case AdvDataField::Appearance: {
                if (fieldSize != (2)) {
                    // do not use sizeof(Appearance), might yield != 2
                    return Error::SizeMissmatch;
                }
                uint16_t rawAppearance;
                std::copy_n(&rawData[offset],
                            sizeof(rawAppearance),
                            reinterpret_cast<uint8_t*>(&rawAppearance));
                Endians::littleToMachine(rawAppearance);
                outData.appearance = static_cast<Appearance>(rawAppearance);
                break;
            }

            case AdvDataField::ManufSpecificData: {
                outData.manufacturerDataSize = fieldSize;
                outData.manufacturerData.reset(
                    new uint8_t[outData.manufacturerDataSize]);
                std::copy(&rawData[offset],
                          &rawData[offset + outData.manufacturerDataSize],
                          outData.manufacturerData.get());
                break;
            }

            case AdvDataField::ShortenedLocalName:
            case AdvDataField::CompleteLocalName: {
                outData.nameSize = fieldSize;
                outData.name.reset(new char[outData.nameSize]);
                std::copy(&rawData[offset],
                          &rawData[offset + outData.nameSize],
                          outData.name.get());
                break;
            }

            default: {
                result = Error::NotFound;
                break;
            }
        }

        offset += fieldSize;
    }
    return result;
}