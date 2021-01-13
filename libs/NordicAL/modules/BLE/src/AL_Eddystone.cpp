/**
 * @file AL_Eddystone.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * 
 * @brief Abstraction for implementation eddystone advertisements.
 * 
 * @version 1.0
 * @date 2020-06-18
 * 
 * @copyright aconno GmbH (c) 2020
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Eddystone.h"

#include <string>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
template <size_t ManufSpecDataSize>
IO::BLE::Eddystone<ManufSpecDataSize>::Eddystone(
    RTOS::milliseconds                       interval,
    TxPower                                  txPower,
    CompanySigId                             companyId,
    std::array<uint8_t, ManufSpecDataSize>&& manufacturerData,
    uint8_t                                  burstCount,
    bool                                     autostart)
    : Advertisement(interval, txPower, burstCount, autostart),
      companyId{companyId}, manufacturerData{manufacturerData}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

template <size_t ManufSpecDataSize>
void IO::BLE::Eddystone<ManufSpecDataSize>::setData(
    const std::array<uint8_t, ManufSpecDataSize>& data)
{
    std::copy(data.cbegin(), data.cend(), manufacturerData.begin());
}

template <size_t ManufSpecDataSize>
const std::array<uint8_t, ManufSpecDataSize>& IO::BLE::Eddystone<
    ManufSpecDataSize>::getData()
{
    return manufacturerData;
}

/**
 * @brief implements advertisement
 * 
 * @tparam ManufSpecDataSize 
 * @param timeout 
 * @return Error::Code 
 */
template <size_t ManufSpecDataSize>
Error::Code IO::BLE::Eddystone<ManufSpecDataSize>::trigger(
    RTOS::milliseconds timeout)
{
    constexpr size_t packageLen = ManufSpecDataSize + sizeof(CompanySigId) +
                                  sizeof(Advertisement::AdvType) + 1;
    uint8_t* package = new uint8_t[packageLen];
    package[0]       = static_cast<uint8_t>(packageLen) - 1;
    package[1] =
        static_cast<uint8_t>(Advertisement::AdvType::ManufacturerSpecific);
    package[2] = reinterpret_cast<uint8_t*>(&companyId)[0];
    package[3] = reinterpret_cast<uint8_t*>(&companyId)[1];
    memcpy(&package[4], manufacturerData.data(), ManufSpecDataSize);

    return queueForAdvertisement(package, packageLen, timeout);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------