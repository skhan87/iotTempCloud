/**
 * @file main.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief 
 * @version 0.1
 * @date 2020-04-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
//------------------------------ INCLUDES -------------------------------------

#include "AL_Log.h"
#include "AL_Port.h"
#include "AL_RTOS.h"
#include "aconnoConfig.h"
// #include "cpBleServices.h"
// #include "cpContactLogger.h"
// #include "cpBeaconScanner.h"
// #include "cpBeaconAdvertiser.h"
// #include "cpUpdateTimer.h"
// #include "cpPeripheralDevices.h"
// #include "cpAccelerometer.h"
// #include "cpDataCommitHandler.h"

/**
 * @brief Function for application main entry
 * 
 */
void main()
{
    Port::init();

//     auto pd = new CP::PeripheralDevices;

//     // start advertisement with default parameters 408ms and 3xburst
//     auto ba = new CP::BeaconAdvertiser {
//         CP::BeaconAdvertiser::SpecificData { static_cast<int8_t>(IO::BLE::TxPowerMax), 0u } };
    
//     auto cl = new CP::ContactLogger { *pd, [] (uint32_t counter) { uptime.updateValue(counter); } };
    
//     // start scanning with default duration 430 ms.
//     auto bs = new CP::BeaconScanner(*cl);

// #ifdef CP_CLINARIS_BEACON
//     auto ac = new CP::Accelerometer {
//         [pd, bs, ba] (bool active) {
//             static auto lastAdvInterval = ba->getInterval();
//             if (active)
//             {
//                 LOG_D("setting interval %d", lastAdvInterval);
//                 ba->setInterval(lastAdvInterval);
//                 bs->startScanning();
//                 pd->blink(CP::PeripheralDevices::LedColour::GREEN, 1, 2000);
//             }
//             else
//             {
//                 lastAdvInterval = ba->getInterval();
//                 LOG_D("keeping interval %d", lastAdvInterval);
//                 ba->setInterval(CP::ADVERTISER_INACTIVITY_INTERVAL);
//                 bs->stopScanning();
//                 pd->blink(CP::PeripheralDevices::LedColour::BLUE, 3);
//             }
//         }
//     };
// #endif
    
//     auto ut = new CP::UpdateTimer(*cl, *bs, *ba, *pd);
//     (void)ut; // avoid unused variable


// #ifdef CP_CLINARIS_BEACON
//     // create observers...
//     auto accThresholdObserver = new CP::Observer<float> {accThreshold,
//         [ac] (float value)
//         {
//             LOG_I("new accelerometer threshold is %d/1000", (value * 1000));
//             ac->setInterruptThreshold(value);
//         } };
//     (void)accThresholdObserver;

//     auto accSleepTimeoutObserver = new CP::Observer<uint32_t> {accSleepTimeout,
//         [ac] (uint32_t value)
//         {
//             LOG_I("new accelerometer timeout is %d", value);
//             ac->setInactivityTimeout(value);
//         } };
//     (void)accSleepTimeoutObserver;
// #endif

//     auto advIntervalObserver = new CP::Observer<uint16_t> {advertisementIntervalMS, 
//         [ba] (auto value) { ba->setInterval(value); } };
//     (void)advIntervalObserver;

//     auto scanDurationObserver = new CP::Observer<uint16_t> {scanIntervalMS, 
//         [bs] (auto value) { bs->setScanDuration(value); } };
//     (void)scanDurationObserver;

//     auto scanIntervalObserver = new CP::Observer<uint16_t> {idleIntervalMS,
//         [bs] (auto value) { bs->setScanInterval(value); } };
//     (void)scanIntervalObserver;

//     auto peripheryControlObserver = new CP::Observer<uint16_t> {peripheryControl,
//         [pd] (auto value) { pd->disableDevices(value); } };
//     (void)peripheryControlObserver;

//     auto groupIdObserver = new CP::Observer<uint16_t> {groupId,
//         [ba] (auto value) { ba->setGroupId(value); } };
//     (void)groupIdObserver;

//     auto rxThresholdObserver = new CP::Observer<int8_t> {rxThreshold,
//         [bs] (auto value) { bs->setRssiThreshold(value); } };
//     (void)rxThresholdObserver;

//     auto txPowerObserver = new CP::Observer<int8_t> {txPowerDb,
//         [ba] (auto value)
//         {
//             LOG_E("Received Tx power: %d", value);

//             IO::BLE::TxPower compValue = IO::BLE::TxPower::m40dB;
//             do {
//                 if (value == static_cast<int8_t>(compValue))
//                 {
//                     ba->setTxPower(compValue);
//                     break;
//                 }
//                 compValue = IO::BLE::getNextTxValue(compValue);
//             } while (compValue != IO::BLE::TxPower::m40dB);

//             txPowerDb.updateValue(static_cast<int8_t>(ba->getTxPower()));
//         } };
//     (void)txPowerObserver;

//     auto requestDataObserver = new CP::DataCommitHandler{requestData,*cl};
//     (void)requestDataObserver;

    RTOS::init();

    // should never get here
    CHECK_ERROR(Error::Unknown);
}
