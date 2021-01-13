/**
 * @file AL_Eddystone.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * 
 * @brief Abstraction for implementation eddystone advertisements.
 * 
 * @warning At the moment only manufacturer specific data is implemented.
 * 
 * @version 1.0
 * @date 2020-06-18
 * 
 * @copyright aconno GmbH (c) 2020
 */

#ifndef __AL_EDDYSTONE_H__
#define __AL_EDDYSTONE_H__

//-------------------------------- PROTOTYPES ---------------------------------

#include <cstddef>
#include <cstdint>

namespace IO::BLE
{
template <size_t ManufSpecDataSize>
class Eddystone;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Advertisement.h"

#include <Error.h>
#include <array>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
template <size_t ManufSpecDataSize>
class Eddystone : public Advertisement {
    // Delete default constructors
    Eddystone()                       = delete;
    Eddystone(const Eddystone& other) = delete;
    Eddystone& operator=(const Eddystone& other) = delete;

public:
    Eddystone(RTOS::milliseconds                       interval,
              TxPower                                  txPower,
              CompanySigId                             companyId,
              std::array<uint8_t, ManufSpecDataSize>&& manufacturerData,
              uint8_t                                  burstCount = 1,
              bool                                     autostart  = true);

    void setData(const std::array<uint8_t, ManufSpecDataSize>& data);
    const std::array<uint8_t, ManufSpecDataSize>& getData();

    virtual Error::Code trigger(
        RTOS::milliseconds timeout = RTOS::Infinity) final;

private:
    uint16_t companyId; /**< Manufacturer id put into the advertisement */
    std::array<uint8_t, ManufSpecDataSize>
        manufacturerData; /**< Manufacturer data put in the manufacturer specific data area */
};

}  // namespace IO::BLE

#include "../src/AL_Eddystone.cpp"

#endif  //__AL_EDDYSTONE_H__