/**
 * @file 	Scanner.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief	Class implementation of the BLE advertisement scanner module.
 *
 * @version	0.2.0
 * @date 	2020-10-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

//--------------------------------- INCLUDES ----------------------------------
#include "AL_BLE.h"
#include "AL_Device.h"
#include "AL_Log.h"
#include "Error.h"
#include "Observable.h"
#include "ble.h"
#include "AL_Task.h"
#include "AL_Queue.h"
#include "BLE_Utility.h"
#include "ParsedAdvData.h"

#include <array>
#include <cstdint>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief	Implementation of a BLE advertisement scanner module.
 * 
 * @details	Utilizes Nordics BLE stack Softdevice to implement scanning for
 * 			BLE advertisements.
 * 
 * 			For each detected advertisement two possibilities exist: either 
 * 			it is sent from the device that has sent advertisements in 
 * 			the past, or this is the first advertisement received from that 
 * 			device.
 * 
 * 			Scanner class utilizes a lifetime list container as a database
 * 			of all devices which have been encountered before. User settings
 * 			allow for selection of two modes: either all devices are retained in
 *          the list, or currently active devices are retained in the list.
 * 
 * 			Scanner also implements filters, which enable class users to declare
 * 			interest in processing only a particular subset of detected
 * 			advertisements.
 * 
 *          Following filters are currently available:
 *              - filtering by manufacturer specific data field
 * 
 * 			Upon detecting an advertisement, advertisement is parsed and then
 *          filters are applied. If advertisement passes filtering, its
 *          broadcasting device MAC address is compared with list of known
 *          devices. If device is found there, it's marked as still active,
 *          and its activity timer is reset.
 * 
 * 			On the other hand, if that particular MAC address is not found
 * 			inside deviceList, scanner will spawn a new Device object, which
 * 			shall add itself into deviceList for future reference.
 * 
 * 			Scanner is observable, meaning users can subscribe to be notified
 * 			every time that a device is discovered. Notification shall contain
 * 			three parameters - reference to device which caused the
 *          notification, boolean signifying whether this device was previously
 *          known or if its new and the parsed advertisment data in form of
 *          a ParsedAdvData instance.
 * 
 * @example Start scanner which scans every 500 ms for the duration of 10 ms,
 *          does not time out and filters advertisements by users data field:
 * 
 *              void main()
 *              {
 *                  Port::init();
 *              
 *                  IO::BLE::Scanner::setFilterByData<1>({0x59}, {0xFF});
 *                  IO::BLE::Scanner::start(500, 10, 0, true);
 *              
 *                  RTOS::init();
 *              
 *                  // should never get here
 *                  CHECK_ERROR(Error::Unknown);
 *              }
 * 
 */
class Scanner : public Patterns::Observable<const Device&,
                                            const bool&,
                                            const ParsedAdvData&> {
    /**  StackSize of the scanner task in sizeof(StackType_t) bytes */
    static constexpr size_t kStackSize = 256;
    /** Size of the buffer to hand to softdevice for receiving advertisements */
    static constexpr size_t kMaxAdvDataSize = BLE_GAP_SCAN_BUFFER_EXTENDED_MIN;
    /** Size of the queue buffering received advertisements */
    static constexpr size_t kAdvertisementQueueSize = 10;

    using TaskType = RTOS::Task<kStackSize, Scanner>;
    friend TaskType;

    // Delete default constructors
    Scanner(const Scanner& other) = delete;
    Scanner& operator=(const Scanner& other) = delete;

    // Befriend classes of importance
    friend IO::BLE::Utility;

public:
    /*--- Public API ---*/
    static Error::Code start(RTOS::milliseconds scanInterval,
                             RTOS::milliseconds scanWindow,
                             RTOS::milliseconds scanTimeout,
                             bool               enableFiltering,
                             bool               deleteDevicesOnTimeout);
    static void        stop();
    static void        printDeviceList();
    static void        enableFiltering();
    static void        disableFiltering();
    static bool        isFilteringEnabled();
    static Scanner&    getInstance();
    template<size_t size>
    static Error::Code setFilterByData(std::array<uint8_t, size>&& filter,
                                       std::array<uint8_t, size>&& mask);

private:
    /**
     * @brief Raw information received from SD scan event.
     * 
     */
    struct RawAdvData {
        RxPower  rssi;
        size_t   dataSize;
        uint8_t* data;
        Address  address;
    };

    /*--- Constructor ---*/
    Scanner();

    /*--- Private API ---*/
    void        onStart();
    void        onRun();
    Error::Code setScanInterval(RTOS::milliseconds timeMs);
    Error::Code setScanWindow(RTOS::milliseconds timeMs);
    Error::Code setScanTimeout(RTOS::milliseconds timeMs);
    bool        advertisementApproved(const ParsedAdvData& advData) const;
    bool        filterByManufacturerData(const ParsedAdvData& advData) const;

    static Error::Code start();
    static void eventHandler(ble_evt_t const* p_ble_evt, void* p_context);
    static void onAdvReport(ble_gap_evt_adv_report_t const* p_adv_report);
    static constexpr uint16_t msToInternalUnits(RTOS::milliseconds time);
    static constexpr RTOS::milliseconds internalUnitsToMs(uint16_t time);

    /*--- Private members ---*/
    uint16_t                   scanInterval; /**< Internal unit of 0,625ms */
    uint16_t                   scanWindow; /**< Internal unit of 0,625ms */
    uint16_t                   scanTimeout; /**< Internal unit of 1s (1000ms) */
    bool                       filteringEnabled;
    size_t                     dataFilterSize;
    std::unique_ptr<uint8_t[]> dataFilter;
    std::unique_ptr<uint8_t[]> dataMask;
    std::array<uint8_t, kMaxAdvDataSize> scanBufferData;

    RTOS::Queue<RawAdvData, kAdvertisementQueueSize>
        advertisementQueue; /**< Queue that stores discovered advertisements for processing */
    TaskType
        task; /**< RTOS task used to execute the scanning, instantiate after all other RTOS objects! */

    static constexpr uint16_t kDefaultScanInterval = 125; /**< 200ms */
    static constexpr uint16_t kDefaultScanWindow   = 31; /**< ~50ms */
    static constexpr uint16_t kDefaultScanTimeout =
        0; /**< No timeout - scan continues until disabled */
};

}  // namespace IO::BLE

template<size_t size>
Error::Code
    IO::BLE::Scanner::setFilterByData(std::array<uint8_t, size>&& filter,
                                      std::array<uint8_t, size>&& mask)
{
    auto& scanner = getInstance();

    if (mask.back() == 0) {
        LOG_W("Invalid data filter mask! Last digit can't be zero!");
        return Error::InvalidParameter;
    }

    scanner.dataFilter = std::make_unique<uint8_t[]>(size);
    scanner.dataMask   = std::make_unique<uint8_t[]>(size);

    if (!scanner.dataFilter || !scanner.dataMask) {
        LOG_W("Failed to create filter arrays!");
        return Error::OutOfResources;
    }

    for (int i = 0; i != size; i++) {
        scanner.dataFilter[i] = filter[i];
        scanner.dataMask[i]   = mask[i];
    }

    scanner.dataFilterSize = size;

    return Error::None;
}

#endif  //__SCANNER_H__
