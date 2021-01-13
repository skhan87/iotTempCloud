/**
 * @file AL_IBeacon.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * 
 * @brief Abstraction for iBeacon advertisements.
 * 
 * @version 1.0
 * @date 2020-06-18
 * 
 * @copyright aconno GmbH (c) 2020
 */

#ifndef __AL_IBEACON_H__
#define __AL_IBEACON_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::BLE
{
class IBeacon;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Advertisement.h"

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class IBeacon : public Advertisement {
    // Delete default constructors
    IBeacon()                     = delete;
    IBeacon(const IBeacon& other) = delete;
    IBeacon& operator=(const IBeacon& other) = delete;

public:
    IBeacon(RTOS::milliseconds        interval,
            TxPower                   txPower,
            uint16_t                  major,
            uint16_t                  minor,
            std::array<uint8_t, 16>&& uuid,
            uint8_t                   burstCount = 1,
            bool                      autostart  = true);

    /** implements Advertisement */
    virtual Error::Code trigger(
        RTOS::milliseconds timeout = RTOS::Infinity) final;

private:
    uint16_t major; /**< Major to indicate collection of devices (e.g. store) */
    uint16_t minor; /**< Minor to indicate device */
    std::array<uint8_t, 16> uuid; /**< Unique identifier */
    static constexpr uint16_t AdvIndicator = 0x1502; /**< Used to indicate an iBeacon in an apple advertisement */
};

}  // namespace IO::BLE

#endif  //__AL_IBEACON_H__