/**
 * @file AconnoBeacon.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Aconno specific beacon advertisement
 * @version 1.0
 * @date 2020-07-30
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __ACONNOBEACON_H__
#define __ACONNOBEACON_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::BLE
{
class AconnoBeacon;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Advertisement.h"

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Aconno specific beacon advertisement.
 * 
 * @details Specified by deserializer 0202.json
 */
class AconnoBeacon : public Advertisement {
    // delete default constructors
    AconnoBeacon()                          = delete;
    AconnoBeacon(const AconnoBeacon& other) = delete;
    AconnoBeacon& operator=(const AconnoBeacon& other) = delete;

public:
    AconnoBeacon(RTOS::milliseconds interval,
                 TxPower            txPower,
                 CompanySigId       companyId,
                 uint8_t            burstCount = 1,
                 bool               autostart  = true,
                 bool               rotateTX   = false);

    virtual Error::Code trigger(
        RTOS::milliseconds timeout = RTOS::Infinity) final;

private:
    uint16_t companyId; /**< Manufacturer id put into the advertisement */
    bool rotateTX; /**< if set, tx power is toggled after each advertisement */

    static constexpr uint8_t PRODUCT_ID =
        0x02; /**< deserializer product id, refer to https://aconno.de/sensorics/formats */
    static constexpr uint8_t PROTOCOL_VERSION =
        0x02; /**< deserializer version, refer to https://aconno.de/sensorics/formats */
};
}  // namespace IO::BLE
#endif  //__ACONNOBEACON_H__