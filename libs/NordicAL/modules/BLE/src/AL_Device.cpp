/**
 * @file	AL_Device.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief 	Implements class which stores information about Devices discovered 
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
 * 			on all devices. Depending on user choice (via kDelistInactive flag), 
 * 			it either stores all devices it came into contact with, or just 
 * 			the ones that are actively broadcasting their messages.
 * 
 * @version	0.1.0
 * @date 	2020-09-16
 *
 * @copyright aconno GmbH (c) 2020
 */

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Device.h"
#include "AL_Log.h"

#include <cstring>

//--------------------------- STRUCTS AND ENUMS -------------------------------

bool IO::BLE::Device::deleteDevicesOnTimeout {true};

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Constructor of Device class objects.
 */
IO::BLE::Device::Device(const std::array<uint8_t, BLE_GAP_ADDR_LEN>& MACaddress,
                        int8_t                                       rssi)
        : address {MACaddress},
          activityTimer(*this, kDefaultActivityTimeout), lastRSSI {rssi},
          node(getList().appendStatic(*this))
{
    activityTimer.start();
}

/**
 * @brief Constructor of Device class Timer object.
 */
IO::BLE::Device::Timer::Timer(Device& device, RTOS::milliseconds timeMs)
        : RTOS::Timer("activityTimer", timeMs, false), device(device)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Device activity status getter function.
 * 
 * @return True if device is active, false if its not.
 */
bool IO::BLE::Device::isActive()
{
    return this->activityTimer.isActive();
}

/**
 * @brief Gets device last advertisement RSSI value.
 * 
 * @return int8_t value of last advertiement RSSI.
 */
int8_t IO::BLE::Device::getLastRSSI() const
{
    return this->lastRSSI;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Sets device activity status to 'active'.
 * 
 * @return None.
 */
void IO::BLE::Device::setToActive()
{
    this->activityTimer.start();
}

/**
 * @brief Sets device last advertisement RSSI value.
 * 
 * @param rssi Set last received signal strength from the device in dBm.
 * 
 * @return None.
 */
void IO::BLE::Device::setLastRSSI(IO::BLE::RxPower rssi)
{
    this->lastRSSI = rssi;
}

/**
 * @brief Implements Device timer timeout handler.
 * 
 * @return None.
 */
void IO::BLE::Device::Timer::onTimer()
{
    if (IO::BLE::Device::deleteDevicesOnTimeout) {
        delete (&device);
    }
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Device lifetime list getter function.
 * 
 * @details Uses eager loading to keep the right order of construction.
 * 			This way statically constructed objects always get a valid 
 * 			LifetimeList instance.
 * 
 * @return Reference to device lifetime list.
 */
Collections::LifetimeList<IO::BLE::Device&>& IO::BLE::Device::getList()
{
    static Collections::LifetimeList<Device&> list {};
    return list;
}

/**
 * @brief Searches the device with the given address in all known devices.
 * 
 * @param address BLE address of the device
 * @return IO::BLE::Device* Pointer to the device or nullptr if not found
 */
IO::BLE::Device* IO::BLE::Device::getByAddress(const IO::BLE::Address& address)
{
    for (auto& device : Device::getList()) {
        if (std::equal(address.cbegin(),
                       address.cend(),
                       device.address.cbegin())) {
            return &device;
        }
    }

    return nullptr;
}

/**
 * @brief Prints list of all detected broadcasting devices.
 * 
 * @details Prints out all devices found in DeviceList as info log messages.
 * 			Devices are saved into the list as their advertisements are
 * 			processed. Depending on parameter setting, list may contains all devices
 * 			ever scanned, or just found devices which are still active.
 * 
 * @param onlyActives Set to true to only print active devices.
 * 
 * @return None.
 */
void IO::BLE::Device::printDeviceList(bool onlyActives)
{
    if (onlyActives) {
        LOG_I("Active BLE device list:");
    } else {
        LOG_I("BLE device list:");
    }

    for (auto& device : IO::BLE::Device::getList()) {
        if (device.isActive()) {
            LOG_I("\t%02X:%02X:%02X:%02X:%02X:%02X,  status: active",
                  device.address[0],
                  device.address[1],
                  device.address[2],
                  device.address[3],
                  device.address[4],
                  device.address[5]);
        } else {
            LOG_I("\t%02X:%02X:%02X:%02X:%02X:%02X,  status: inactive",
                  device.address[0],
                  device.address[1],
                  device.address[2],
                  device.address[3],
                  device.address[4],
                  device.address[5]);
        }
    }
    LOG_I("--------------------------------------------");
}

/**
 * @brief Set whether to delete discovered devices after kDefaultActivityTimeout.
 * 
 * @details Devices are heap allocated. Thereby deleting discovered device
 * after some time avoids an overflow of heap memory.
 * 
 * @param active Deletes old devices when true.
 */
void IO::BLE::Device::setDeleteDevicesOnTimeout(bool active)
{
    deleteDevicesOnTimeout = active;
}