/**
 * @file	AL_Device.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief 	Implements class which stores information about
 *          Devices discovered during scanning. 
 * @version	0.1.0
 * @date 	2020-09-16
 *
 * @copyright aconno GmbH (c) 2020
 */

#ifndef __AL_DEVICE_H__
#define __AL_DEVICE_H__

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Timer.h"
#include "LifetimeList.h"
#include "ble_gap.h"
#include "AL_BLE.h"

#include <array>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
/**
 * @brief Implements class which stores information about Devices discovered 
 * 			during scanning.
 * 
 * @details	While operating in BLE Central mode, a device scans its surroundings
 * 			in search of BLE advertisements, used by BLE peripherals to 
 * 			broadcast information or provide way to connect to them. Central can
 * 			extract useful information about the device broadcasting 
 * 			the advertisement once it reads the advertisement. Purpose of this
 * 			class is to save that information for eventual later use.
 * 
 * 			By utilizing a lifetime linked list, this class stores information
 * 			on all devices. Depending on user choice (via
 *          setDeleteDevicesOnTimeout function), it either stores all devices
 *          it came into contact with, or just the ones that are actively
 *          broadcasting their messages. All older devices will gett automatically
 *          deleted.
 * 
 */
class Device {
    // Delete default constructors
    Device()                    = delete;
    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

    friend class Scanner;

    // Implements device activity timer
    class Timer : public RTOS::Timer {
        // Delete default constructors
        Timer()                   = delete;
        Timer(const Timer& other) = delete;
        Timer& operator=(const Timer& other) = delete;

    public:
        /*--- Constructors ---*/
        Timer(Device&            device,
              RTOS::milliseconds timeMs = kDefaultActivityTimeout);

    private:
        /*--- Private API ---*/
        virtual void onTimer() final;

        /*--- Private members ---*/
        Device& device;
    };

public:
    /*--- Constructors ---*/
    Device(const std::array<uint8_t, BLE_GAP_ADDR_LEN>& MACaddress,
           RxPower                                      rssi);

    /*--- Public API ---*/
    bool             isActive();
    IO::BLE::RxPower getLastRSSI() const;

    static Collections::LifetimeList<Device&>& getList();
    static Device* getByAddress(const Address& address);
    static void    printDeviceList(bool onlyActives);
    static void    setDeleteDevicesOnTimeout(bool active);

    /*--- Public members ---*/
    Address address; /**< 48-bit BT MAC address*/

private:
    /*--- Private members ---*/
    Timer                                    activityTimer;
    IO::BLE::RxPower                         lastRSSI;
    Collections::LifetimeList<Device&>::Node node; /**< DeviceList node */

    void setToActive();
    void setLastRSSI(RxPower rssi);

    static bool deleteDevicesOnTimeout;

    /*--- Private constants ---*/
    static constexpr uint16_t kDefaultActivityTimeout =
        12000; /**< Max. adv. interval is 10.24s according to BLE specifications */
};

}  // namespace IO::BLE

#endif  //__AL_DEVICE_H__