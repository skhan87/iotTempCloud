/**
 * @file 	Scanner.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief	Class implementation of the BLE advertisement scanner module.
 *
 * @version	0.2.0
 * @date 	2020-10-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------
#include "Endians.h"
#include "PortUtility.h"
#include "Scanner.h"
#include "nrf_sdh_ble.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Constructor of Scanner class object.
 */
IO::BLE::Scanner::Scanner()
        : scanInterval {kDefaultScanInterval}, scanWindow {kDefaultScanWindow},
          scanTimeout {kDefaultScanTimeout}, filteringEnabled {false},
          dataFilter {nullptr}, dataMask {nullptr}, advertisementQueue {"Scanne"
                                                                        "rAdvQu"
                                                                        "eue"},
          task {*this, "scannerTask", 3}
{
    NRF_SDH_BLE_OBSERVER(m_ble_scanner_observer,
                         NRF_BLE_SCAN_OBSERVER_PRIO,
                         IO::BLE::Scanner::eventHandler,
                         nullptr);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Starting scanning for BLE advertisements.
 * 
 * @details Starts scanning module with values given by user.
 *          Once called, it spawns itself in the static context.
 * 
 * @param   scanInterval Set how often scanner performs the scan in [ms].
 * @param   scanWindow Set how long scanner keeps scanner at each interval,[ms].
 * @param   scanTimeout Set limit in [ms] after which scanning will no longer
 *                      be performed at set interval. To scan again, scanner
 *                      has to be started again. Set to '0' to scan forever.
 * @param   enableFiltering Allow set filters to be used.
 * @param   deleteDevicesOnTimeout Set to true to delete devices that did
 *                                 not get received for 
 *                                 IO::BLE::Device::kDefaultActivityTimeout.
 *                                 Defaults to true to free heap.
 * 
 * @return  Error code if failed to start, Error::None if successful
 * 
 * @example Scans every 500 ms, for 10 ms, and does not timeout.
 *          It filters scans, passing through only those which have first byte 
 *          of the manufacturer-specific data field exactly equal to 0x59.
 * 
 *          #include "AL_Port.h"
 *          #include "AL_RTOS.h"
 *          #include "Scanner.h"
 *          
 *          void main()
 *          {
 *              Port::init();
 *          
 *              IO::BLE::Scanner::setFilterByData<1>({0x59}, {0xFF});
 *              IO::BLE::Scanner::start(500, 10, 0, true);
 *          
 *              RTOS::init();
 *          
 *              // should never get here
 *              CHECK_ERROR(Error::Unknown);
 *          }
 */
Error::Code IO::BLE::Scanner::start(RTOS::milliseconds scanInterval,
                                    RTOS::milliseconds scanWindow,
                                    RTOS::milliseconds scanTimeout,
                                    bool               enableFiltering,
                                    bool               deleteDevicesOnTimeout)
{
    auto& scanner = getInstance();

    RETURN_ON_ERROR(scanner.setScanInterval(scanInterval));
    RETURN_ON_ERROR(scanner.setScanWindow(scanWindow));
    RETURN_ON_ERROR(scanner.setScanTimeout(scanTimeout));

    if (enableFiltering) {
        IO::BLE::Scanner::enableFiltering();
    } else {
        IO::BLE::Scanner::disableFiltering();
    }

    IO::BLE::Device::setDeleteDevicesOnTimeout(deleteDevicesOnTimeout);

    return Scanner::start();
}

/**
 * @brief Stops scanning for advertisements.
 * 
 * @details It is ok to ignore the function return value here, because this 
 * 			function can return NRF_SUCCESS or NRF_ERROR_INVALID_STATE, when 
 * 			app is not in the scanning state.
 * 
 * @return None.
 */
void IO::BLE::Scanner::stop()
{
    sd_ble_gap_scan_stop();
}

/**
 * @brief Enable filtering received advertisements using user filters.
 * 
 * @return None.
 */
void IO::BLE::Scanner::enableFiltering()
{
    getInstance().filteringEnabled = true;
}

/**
 * @brief Disable filtering received advertisements using user filters.
 * 
 * @return None.
 */
void IO::BLE::Scanner::disableFiltering()
{
    getInstance().filteringEnabled = false;
}

/**
 * @brief Query if filtering received advertisements using user filters is
 * 			enabled.
 * 
 * @return True if filtering is enabled, false otherwise.
 */
bool IO::BLE::Scanner::isFilteringEnabled()
{
    return getInstance().filteringEnabled;
}

/**
 * @brief Scanner instance getter function.
 * 
 * @details Uses eager loading to keep the right order of construction.
 * 			This way statically constructed objects always get a valid 
 * 			Scanner instance.
 * 
 * @return Reference to Scanner object.
 */
IO::BLE::Scanner& IO::BLE::Scanner::getInstance()
{
    static IO::BLE::Scanner instance {};
    return instance;
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Task implementation
 * 
 */
void IO::BLE::Scanner::onStart()
{
    // do nothing
}

/**
 * @brief Implementation of scanner task
 * 
 * @details Scanner task blocks on advertisement queue, waiting for scanner to
 * 			detect advertisements. Once placed in queue, advertisements are
 * 			taken from the queue and processed one by one by this task function.
 * 
 * @return None.
 */
void IO::BLE::Scanner::onRun()
{
    RawAdvData rawAdv {};

    // Block until there is advertisement to process
    Error::Code retVal = advertisementQueue.receive(rawAdv);
    if (retVal != Error::None) {
        LOG_W("Scanner could not receive from queue");
        return;
    }

    bool  newDevice = false;
    auto* device    = Device::getByAddress(rawAdv.address);
    if (!device) {
        // If the device is not yet known, create
        newDevice = true;
        device    = new Device {rawAdv.address, rawAdv.rssi};
    } else {
        device->setToActive();
        device->setLastRSSI(rawAdv.rssi);
    }

    std::unique_ptr<uint8_t[]> uniqueData {rawAdv.data};
    ParsedAdvData              parsed {};
    retVal = ParsedAdvData::parseRawData(parsed,
                                         std::move(uniqueData),
                                         rawAdv.dataSize);
    if (retVal == Error::NotFound) {
        LOG_D("Advertisement Package contains not implemented fields "
              "%02X:%02X:%02X:%02X:%02X:%02X",
              rawAdv.address[0],
              rawAdv.address[1],
              rawAdv.address[2],
              rawAdv.address[3],
              rawAdv.address[4],
              rawAdv.address[5]);
    } else if (retVal != Error::None) {
        LOG_W("Could not parse adv package from %02X:%02X:%02X:%02X:%02X:%02X",
              rawAdv.address[0],
              rawAdv.address[1],
              rawAdv.address[2],
              rawAdv.address[3],
              rawAdv.address[4],
              rawAdv.address[5]);
        LOG_W("Parsing failed with: %u", retVal);
        return;
    }

    if (advertisementApproved(parsed)) {
        // Find out who sent the advertisement, or record new sender device
        trigger(*device, newDevice, parsed);
    }
}

/**
 * @brief Scanning interval setter function.
 * 
 * @details Performs conversion from time interval in milliseconds to 
 * 			corresponding value in modules internal units. Nordic softdevice 
 *          internally uses interval in 625 us units.
 * 
 *          Minimum supported interval is 2.5 ms.
 *          Maximum supported interval is 40 959 375 sec.
 * 
 * @param	timeMs	Scanning interval in milliseconds.
 * 
 * @return None.
 */
Error::Code IO::BLE::Scanner::setScanInterval(RTOS::milliseconds timeMs)
{
    // Out of bounds checks
    if (timeMs > internalUnitsToMs(BLE_GAP_SCAN_INTERVAL_MAX)) {
        return Error::MaxReached;
    } else if (timeMs < internalUnitsToMs(BLE_GAP_SCAN_INTERVAL_MIN)) {
        return Error::MinReached;
    }

    scanInterval = msToInternalUnits(timeMs);
    LOG_I("Scanner scan interval set to %d ms", timeMs);
    return Error::None;
}

/**
 * @brief Scanning window interval setter function.
 * 
 * @details Performs conversion from time interval in milliseconds to 
 * 			corresponding value in modules internal units. Nordic softdevice
 *          internally uses scanning window value in 625 us units.
 * 
 *          Minimum supported scan window is 2.5 ms.
 *          Maximum supported scan window is 40 959 375 sec.
 * 
 * @param	timeMs	Scanning window interval in milliseconds.
 * 
 * @return None.
 */
Error::Code IO::BLE::Scanner::setScanWindow(RTOS::milliseconds timeMs)
{
    if (timeMs > internalUnitsToMs(BLE_GAP_SCAN_WINDOW_MAX)) {
        return Error::MaxReached;
    } else if (timeMs < internalUnitsToMs(BLE_GAP_SCAN_WINDOW_MIN)) {
        return Error::MinReached;
    }

    scanWindow = msToInternalUnits(timeMs);
    LOG_I("Scanner scan window set to %d ms", timeMs);
    return Error::None;
}

/**
 * @brief Scanning timeout setter function.
 * 
 * @details Performs conversion from time interval in milliseconds to 
 * 			corresponding value in modules internal units. Nordic softdevice
 *          internally uses timeout in 10 ms units.
 * 
 *          Minimum scan timeout value is 10 ms.
 *          If this function is called with value 0, module will continue to
 *          scan forever.
 * 
 * @param	timeMs	Scanning timeout in milliseconds.
 * 
 * @return None.
 */
Error::Code IO::BLE::Scanner::setScanTimeout(RTOS::milliseconds timeMs)
{
    if (timeMs < 0) {
        return Error::MinReached;
    } else if (timeMs > (0xFFFF * 1000)) {
        // maximum uint16_t value times s/ms
        return Error::MaxReached;
    }

    scanTimeout = static_cast<uint16_t>(timeMs / 1000);
    if (timeMs == 0) {
        LOG_I("Scanner scan timeout disabled. Scanner will scan forever.");
    } else {
        LOG_I("Scanner scan timeout set to %d ms", timeMs);
    }
    return Error::None;
}

/**
 * @brief	Verified wheather the advertisement should be passed on or discarded.
 * 
 * @details If a filter is active, two conditions have to be met:
 * 				- field which is filtered must exist in the advertisement
 * 				- field which is filtered must match filter
 * 
 * @param	advData Readily parsed advertisement data.
 * 
 * @return True if advertisement approved for processing, false otherwise.
 */
bool IO::BLE::Scanner::advertisementApproved(const ParsedAdvData& advData) const
{
    if (!isFilteringEnabled()) {
        // All advs are approved
        return true;
    }

    if (!filterByManufacturerData(advData)) {
        return false;
    }

    // Advertisement passed filtering.
    return true;
}

/**
 * @brief	Applies user filter on manufacturer specific data field of the
 * 			advertisement.
 * 
 * @details Applies user-set filter mask and compares value with the value
 * 			requested by user.
 * 
 * @param	advData[in] Data contents of received advertisement.
 * 
 * @return True if advertisement passes filter, false otherwise.
 */
bool IO::BLE::Scanner::filterByManufacturerData(
    const ParsedAdvData& advData) const
{
    if (!dataFilter) {
        // No filter data set, do not apply
        LOG_W("Scanner filter activated but not set.");
        return true;
    }

    if (advData.manufacturerDataSize < dataFilterSize) {
        // Data is smaller than filter, therefore cannot match it
        return false;
    }

    // Compare manufacturer data field with filter, byte by byte
    for (size_t i = 0; i != dataFilterSize; i++) {
        if (dataMask[i] == 0x0) {
            // Any value for this field is OK
            continue;
        } else if ((advData.manufacturerData[i] & dataMask[i]) !=
                   (dataFilter[i] & dataMask[i])) {
            // Value mismatch in field mandated by filter, discard this adv.
            return false;
        }
    }

    // Filter matched
    return true;
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Starting scanning for BLE advertisements.
 * 
 * @details Starts scanning module using values previously set by the user.
 *          This method is meant to be used internally by the library once
 *          user has called the public start() method.
 * 
 * @return Error code if failed to start, Error::None if successful
 */
Error::Code IO::BLE::Scanner::start()
{
    LOG_I("Starting BLE scanner.");
    auto& scanner = getInstance();

    Scanner::stop();

    // Prepare and start the scanning.
    ble_gap_scan_params_t scanParams = {
        .extended               = false, /**< Extended advs are not supported */
        .report_incomplete_evts = false, /**< Extended advs are not supported */
        .active                 = false, /**< Scan requests are not supported */
        .filter_policy =
            BLE_GAP_SCAN_FP_ACCEPT_ALL, /**< Whitelist is not implemented */
        .scan_phys    = BLE_GAP_PHY_1MBPS,
        .interval     = scanner.scanInterval,
        .window       = scanner.scanWindow,
        .timeout      = scanner.scanTimeout,
        .channel_mask = {0}};

    ble_data_t scanBuffer = {
        .p_data = scanner.scanBufferData.data(),
        .len    = static_cast<uint16_t>(scanner.scanBufferData.size())};

    ret_code_t errCode = sd_ble_gap_scan_start(&scanParams, &scanBuffer);

    // It is okay to ignore invalid state error, as scan was stopped earlier.
    if ((errCode != NRF_ERROR_INVALID_STATE) && (errCode != NRF_SUCCESS)) {
        return Port::Utility::getError(errCode);
    }

    if (isFilteringEnabled()) {
        LOG_I("BLE scanner started, filtering enabled.");
    } else {
        LOG_I("BLE scanner started, filtering disabled");
    }

    return Error::None;
}

/**
 * @brief Softdevice BLE scanning event handler.
 * 
 * @details Handles softdevice BLE stack events related to BLE scanning. 
 * 
 * @return None.
 */
void IO::BLE::Scanner::eventHandler(ble_evt_t const* p_ble_evt, void* p_context)
{
    ble_gap_evt_t const* p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_ADV_REPORT:
            onAdvReport(&p_gap_evt->params.adv_report);
            break;
    }
}

/**
 * @brief Function for handling the advertising report BLE event.
 * 
 * @details Advertising report event denotes that an advertisement has been
 * 			detected and read by the scanner. Required information is extracted
 * 			from the report and copied to advertisement queue.
 * 
 * 			From there, advertisements are processed without blocking scanner
 * 			functionality.
 * 
 * 			Once advertisement is queued, scanner is restarted.
 *
 * @param p_adv_report  Advertising report from the SoftDevice.
 * 
 * @return None.
 */
void IO::BLE::Scanner::onAdvReport(ble_gap_evt_adv_report_t const* p_adv_report)
{
    size_t   dataSize = p_adv_report->data.len;
    uint8_t* data     = nullptr;
    Address  address {};

    if (dataSize > 0) {
        data = new uint8_t[dataSize];
    }

    // Fill scanner data package using scanned advertisement
    std::copy(&p_adv_report->data.p_data[0],
              &p_adv_report->data.p_data[dataSize],
              data);

    std::copy(&p_adv_report->peer_addr.addr[0],
              &p_adv_report->peer_addr.addr[sizeof(Address)],
              address.begin());

    // Copy to queue
    Error::Code retVal = getInstance().advertisementQueue.send(
        {p_adv_report->rssi, dataSize, data, std::move(address)});
    if (retVal != Error::Code::None) {
        LOG_E("Scanner - advertisement queue enqueue failed!: %u", retVal);
        // heap memor needs to be freed here
        if (data) {
            delete[](data);
        }
    }

    // Continue scanning
    ble_data_t scanBuffer = {
        .p_data = getInstance().scanBufferData.data(),
        .len    = static_cast<uint16_t>(getInstance().scanBufferData.size())};
    ret_code_t errCode = sd_ble_gap_scan_start(NULL, &scanBuffer);
    if (errCode != NRF_SUCCESS) {
        LOG_W("BLE scanner error %u", Port::Utility::getError(errCode));
    }
}

/**
 * @brief Converts milliseconds to internal units.
 * 
 * @warning Range checks have to be done by the caller.
 * 
 * @details These units are used for scan interval and scan window.
 * 
 * @param time Time in milliseconds.
 * @return constexpr uint16_t Time in internal units of 0,625ms.
 */
constexpr uint16_t IO::BLE::Scanner::msToInternalUnits(RTOS::milliseconds time)
{
    return time * 8 / 5;
}

/**
 * @brief Converts internal units to milliseconds
 * 
 * @details These units are used for scan interval and scan window.
 * 
 * @param time Time in internal units of 0,625ms
 * @return constexpr RTOS::milliseconds time in ms
 */
constexpr RTOS::milliseconds IO::BLE::Scanner::internalUnitsToMs(uint16_t time)
{
    return static_cast<RTOS::milliseconds>(time) * 5 / 8;
}