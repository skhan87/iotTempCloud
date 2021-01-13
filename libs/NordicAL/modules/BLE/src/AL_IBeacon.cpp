/**
 * @file AL_IBeacon.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for iBeacons
 * @version 1.0
 * @date 2020-06-18
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_IBeacon.h"

#include <AL_Log.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
IO::BLE::IBeacon::IBeacon(RTOS::milliseconds        interval,
                          TxPower                   txPower,
                          uint16_t                  major,
                          uint16_t                  minor,
                          std::array<uint8_t, 16>&& uuid,
                          uint8_t                   burstCount,
                          bool                      autostart)
    : Advertisement(interval, txPower, burstCount, autostart), major(major),
      minor(minor), uuid(uuid)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

Error::Code IO::BLE::IBeacon::trigger(RTOS::milliseconds timeout)
{
    constexpr size_t packageLen =
        sizeof(Advertisement::AdvType) + sizeof(CompanySigId) +
        sizeof(AdvIndicator) + sizeof(uuid) + sizeof(major) + sizeof(minor) + 2;
    uint8_t* package = new uint8_t[packageLen];

    const auto companyId = CompanySigId::Apple;

    size_t i     = 0;
    package[i++] = static_cast<uint8_t>(packageLen) - 1;
    package[i++] =
        static_cast<uint8_t>(Advertisement::AdvType::ManufacturerSpecific);
    package[i++] = reinterpret_cast<const uint8_t*>(&companyId)[0];
    package[i++] = reinterpret_cast<const uint8_t*>(&companyId)[1];
    package[i++] = reinterpret_cast<const uint8_t*>(&AdvIndicator)[0];
    package[i++] = reinterpret_cast<const uint8_t*>(&AdvIndicator)[1];
    memcpy(&package[i], uuid.data(), sizeof(uuid));
    i += 16;
    package[i++] = reinterpret_cast<uint8_t*>(&major)[1];
    package[i++] = reinterpret_cast<uint8_t*>(&major)[0];
    package[i++] = reinterpret_cast<uint8_t*>(&minor)[1];
    package[i++] = reinterpret_cast<uint8_t*>(&minor)[0];
    package[i++] = static_cast<int8_t>(getStdRx(getTXPower()));

    return queueForAdvertisement(package, packageLen, timeout);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------