/**
 * @file AconnoBeacon.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief standard aconno beacon format
 * @version 1.0
 * @date 2020-07-30
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AconnoBeacon.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new IO::BLE::AconnoBeacon::AconnoBeacon advertisement
 * 
 * @param interval interval in which advertisements are sent.
 * @param txPower power to advertise with. This will rotate when rotateTX is set.
 * @param companyId comany ID
 * @param burstCount number of packages sent per interval
 * @param autostart starts on its own after beeing created
 * @param rotateTX rotates the TX powers while advertising
 */
IO::BLE::AconnoBeacon::AconnoBeacon(RTOS::milliseconds interval,
                                    TxPower            txPower,
                                    CompanySigId       companyId,
                                    uint8_t            burstCount,
                                    bool               autostart,
                                    bool               rotateTX)
    : Advertisement(interval, txPower, burstCount, autostart),
      companyId(companyId), rotateTX(rotateTX)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief implements Advertisement
 * 
 * @param timeout 
 * @return Error::Code 
 */
Error::Code IO::BLE::AconnoBeacon::trigger(RTOS::milliseconds timeout)
{
    constexpr size_t packageLen =
        sizeof(CompanySigId) + sizeof(Advertisement::AdvType) +
        sizeof(Config::ACONNO_ID) + sizeof(PRODUCT_ID) +
        sizeof(PROTOCOL_VERSION) + sizeof(RxPower) + sizeof(RxPower) + 1;

    if (rotateTX) {
        // iterate over all TX powers
        CHECK_ERROR(setTXPower(getNextTxValue(getTXPower())));
    }

    uint8_t* package           = new uint8_t[packageLen];
    size_t   i                 = 0;
    auto     stdRx             = getStdRx(getTXPower());
    auto     stdRxStdDeviation = getStdRxStdDeviation(getTXPower());

    package[i++] = static_cast<uint8_t>(packageLen) - 1;
    package[i++] =
        static_cast<uint8_t>(Advertisement::AdvType::ManufacturerSpecific);
    package[i++] = reinterpret_cast<uint8_t*>(&companyId)[0];
    package[i++] = reinterpret_cast<uint8_t*>(&companyId)[1];
    package[i++] = Config::ACONNO_ID;
    package[i++] = PRODUCT_ID;
    package[i++] = PROTOCOL_VERSION;
    package[i++] = *reinterpret_cast<uint8_t*>(&stdRx);
    package[i++] = *reinterpret_cast<uint8_t*>(&stdRxStdDeviation);

    return queueForAdvertisement(package, packageLen, timeout);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------