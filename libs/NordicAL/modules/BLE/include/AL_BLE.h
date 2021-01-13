/**
 * @file AL_BLE.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief Common BLE declarations.
 * 
 * @version 1.0
 * @date 2020-06-04
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_BLE_H__
#define __AL_BLE_H__

//--------------------------------- INCLUDES ----------------------------------

#include "ble_gap.h"

#include <Error.h>
#include <cstdint>
#include <array>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- GLOBAL VARIABLES -------------------------------

//----------------------------- EXPOSED FUNCTIONS -----------------------------
namespace IO::BLE
{
// structs and enums
/**
 * @brief Enumeration of valid radio TX power values
 */
enum class TxPower : int8_t {
    m40dB = -40,
    m20dB = -20,
    m16dB = -16,
    m12dB = -12,
    m8dB  = -8,
    m4dB  = -4,
    p0dB  = 0,
    p3dB  = 3,
    p4dB  = 4,
#if defined(NRF52832_XXAA)
// thats it
#elif defined(NRF52840_XXAA)
    p8dB = 8,
#else
#error unknown port
#endif
};

// Max. transmission power (platform-dependant)
#if defined(NRF52832_XXAA)
constexpr TxPower TxPowerMax = TxPower::p4dB;
#elif defined(NRF52840_XXAA)
constexpr TxPower TxPowerMax = TxPower::p8dB;
#else
#error unknown port
#endif

/**
 * @brief Received transmission power
 * 
 */
using RxPower = int8_t;

/**
 * @brief Data type for BLE addresses
 * 
 */
using Address = std::array<uint8_t, BLE_GAP_ADDR_LEN>;

/** 
 * @brief Company identifiers defined by SIG.
 * 
 * @details Manufacturer id put into the advertisement.
 */
enum CompanySigId : uint16_t {
    Apple               = 0x004C,
    NordicSemiconductor = 0x0059,
};

/**
 * @brief Returns the standard RSSI measured 1m from transmitter.
 * 
 * @param sendPower sending power in decibel
 * @return constexpr RxPower 
 */
constexpr RxPower getStdRx(TxPower sendPower)
{
    switch (sendPower) {
        case TxPower::m40dB:
            return -100;
        case TxPower::m20dB:
            return -85;
        case TxPower::m16dB:
            return -82;
        case TxPower::m12dB:
            return -78;
        case TxPower::m8dB:
            return -74;
        case TxPower::m4dB:
            return -71;
        case TxPower::p0dB:
            return -68;
        case TxPower::p3dB:
            return -66;
        case TxPower::p4dB:
            return -65;
#if defined(NRF52832_XXAA)
// thats it
#elif defined(NRF52840_XXAA)
        case TxPower::p8dB:
            return -61;
#else
#error port not implemented
#endif
        default:
            CHECK_ERROR(Error::NotFound);
            return 127;
    }
}

/**
 * @brief Get the Std Rx Standard Deviation
 * 
 * @param sendPower sending power in decibel
 * @return constexpr int8_t 
 */
constexpr RxPower getStdRxStdDeviation(TxPower sendPower)
{
    switch (sendPower) {
        case TxPower::m40dB:
            return 6;
        case TxPower::m20dB:
            return 6;
        case TxPower::m16dB:
            return 6;
        case TxPower::m12dB:
            return 6;
        case TxPower::m8dB:
            return 6;
        case TxPower::m4dB:
            return 6;
        case TxPower::p0dB:
            return 6;
        case TxPower::p3dB:
            return 6;
        case TxPower::p4dB:
            return 6;
#if defined(NRF52832_XXAA)
// thats it
#elif defined(NRF52840_XXAA)
        case TxPower::p8dB:
            return 6;
#else
#error port not implemented
#endif
        default:
            CHECK_ERROR(Error::NotFound);
            return 127;
    }
}

/**
 * @brief iterates over the tx powers and wraps around on maximum TX to minimum.
 * 
 * @param power 
 * @return constexpr TxPower 
 */
constexpr TxPower getNextTxValue(TxPower power)
{
    switch (power) {
        case TxPower::m40dB:
            return TxPower::m20dB;
        case TxPower::m20dB:
            return TxPower::m16dB;
        case TxPower::m16dB:
            return TxPower::m12dB;
        case TxPower::m12dB:
            return TxPower::m8dB;
        case TxPower::m8dB:
            return TxPower::m4dB;
        case TxPower::m4dB:
            return TxPower::p0dB;
        case TxPower::p0dB:
            return TxPower::p3dB;
        case TxPower::p3dB:
            return TxPower::p4dB;
        case TxPower::p4dB:
#if defined(NRF52832_XXAA)
            return TxPower::m40dB;
#elif defined(NRF52840_XXAA)
            return TxPower::p8dB;
        case TxPower::p8dB:
            return TxPower::m40dB;
#else
#error port not implemented
#endif
        default:
            CHECK_ERROR(Error::NotFound);
            return TxPower::m40dB;
    }
}

/* LE - Low Energy
 * BR/EDR - Bluetooth Basic Rate/Enhanced Data Rate (i.e. Bluetooth Classic)
 */
enum class Flags : uint8_t {
    None = 0xFF, /**< Do not include flags. */
    LimitedDiscoverableMode =
        BLE_GAP_ADV_FLAG_LE_LIMITED_DISC_MODE, /**< LE Limited Discoverable Mode. */
    GeneralDiscoverableMode =
        BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE, /**< LE General Discoverable Mode. */
    BTClassicNotSupported =
        BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED, /**< BR/EDR not supported. */
    BLEAndBTClassicSupportedController =
        BLE_GAP_ADV_FLAG_LE_BR_EDR_CONTROLLER, /**< Simultaneous LE and BR/EDR, Controller. */
    BLEAndBTClassicSupportedHost =
        BLE_GAP_ADV_FLAG_LE_BR_EDR_HOST, /**< Simultaneous LE and BR/EDR, Host. */
    LimitedDiscoverableModeWithBLEOnly =
        BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE, /**< LE Limited Discoverable Mode, BR/EDR not supported. */
    GeneralDiscoverableModeWithBLEOnly =
        BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE /**< LE General Discoverable Mode, BR/EDR not supported. */
};

enum class Appearance : uint16_t {
    None            = 0xFF, /**< Do not include appearance. */
    Unknown         = BLE_APPEARANCE_UNKNOWN, /**< Unknown. */
    GenericPhone    = BLE_APPEARANCE_GENERIC_PHONE, /**< Generic Phone. */
    GenericComputer = BLE_APPEARANCE_GENERIC_COMPUTER, /**< Generic Computer. */
    GenericWatch    = BLE_APPEARANCE_GENERIC_WATCH, /**< Generic Watch. */
    SportsWatch =
        BLE_APPEARANCE_WATCH_SPORTS_WATCH, /**< Watch: Sports Watch. */
    GenericClock   = BLE_APPEARANCE_GENERIC_CLOCK, /**< Generic Clock. */
    GenericDisplay = BLE_APPEARANCE_GENERIC_DISPLAY, /**< Generic Display. */
    GenericRemoteControl =
        BLE_APPEARANCE_GENERIC_REMOTE_CONTROL, /**< Generic Remote Control. */
    GenericEyeGlasses =
        BLE_APPEARANCE_GENERIC_EYE_GLASSES, /**< Generic Eye-glasses. */
    GenericTag     = BLE_APPEARANCE_GENERIC_TAG, /**< Generic Tag. */
    GenericKeyring = BLE_APPEARANCE_GENERIC_KEYRING, /**< Generic Keyring. */
    GenericMediaPlayer =
        BLE_APPEARANCE_GENERIC_MEDIA_PLAYER, /**< Generic Media Player. */
    GenericBarcodeScanner =
        BLE_APPEARANCE_GENERIC_BARCODE_SCANNER, /**< Generic Barcode Scanner. */
    GenericThermometer =
        BLE_APPEARANCE_GENERIC_THERMOMETER, /**< Generic Thermometer. */
    EarThermometer = BLE_APPEARANCE_THERMOMETER_EAR, /**< Thermometer: Ear. */
    GenericHeartRateSensor =
        BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR, /**< Generic Heart rate Sensor. */
    BeltHeartRateSensor =
        BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT, /**< Heart Rate Sensor: Heart Rate Belt. */
    GenericBloodPressure =
        BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE, /**< Generic Blood Pressure. */
    BloodPressureOnArm =
        BLE_APPEARANCE_BLOOD_PRESSURE_ARM, /**< Blood Pressure: Arm. */
    BloodPressureOnWrist =
        BLE_APPEARANCE_BLOOD_PRESSURE_WRIST, /**< Blood Pressure: Wrist. */
    GenericHID =
        BLE_APPEARANCE_GENERIC_HID, /**< Human Interface Device (HID). */
    KeyboardHID = BLE_APPEARANCE_HID_KEYBOARD, /**< Keyboard (HID Subtype). */
    MouseHID    = BLE_APPEARANCE_HID_MOUSE, /**< Mouse (HID Subtype). */
    JoystickHID = BLE_APPEARANCE_HID_JOYSTICK, /**< Joystick (HID Subtype). */
    GamepadHID  = BLE_APPEARANCE_HID_GAMEPAD, /**< Gamepad (HID Subtype). */
    DigitizerTabletHID =
        BLE_APPEARANCE_HID_DIGITIZERSUBTYPE, /**< Digitizer Tablet (HID Subtype). */
    CardReaderHID =
        BLE_APPEARANCE_HID_CARD_READER, /**< Card Reader (HID Subtype). */
    DigitalPenHID =
        BLE_APPEARANCE_HID_DIGITAL_PEN, /**< Digital Pen (HID Subtype). */
    BarcodeHID =
        BLE_APPEARANCE_HID_BARCODE, /**< Barcode Scanner (HID Subtype). */
    GenericGlucoseMeter =
        BLE_APPEARANCE_GENERIC_GLUCOSE_METER, /**< Generic Glucose Meter. */
    GenericRunningWalkingSensor =
        BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR, /**< Generic Running Walking Sensor. */
    RunningWalkingSensorInShoe =
        BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE, /**< Running Walking Sensor: In-Shoe. */
    RunningWalkingSensorOnShoe =
        BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE, /**< Running Walking Sensor: On-Shoe. */
    RunningWalkingSensorOnHip =
        BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP, /**< Running Walking Sensor: On-Hip. */
    GenericCycling = BLE_APPEARANCE_GENERIC_CYCLING, /**< Generic Cycling. */
    CyclingComputer =
        BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER, /**< Cycling: Cycling Computer. */
    CyclingSpeedSensor =
        BLE_APPEARANCE_CYCLING_SPEED_SENSOR, /**< Cycling: Speed Sensor. */
    CyclingCadenceSensor =
        BLE_APPEARANCE_CYCLING_CADENCE_SENSOR, /**< Cycling: Cadence Sensor. */
    CyclingPowerSensor =
        BLE_APPEARANCE_CYCLING_POWER_SENSOR, /**< Cycling: Power Sensor. */
    CyclingSpeedCadenceSensor =
        BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR, /**< Cycling: Speed and Cadence Sensor. */
    GenericPulseOximeter =
        BLE_APPEARANCE_GENERIC_PULSE_OXIMETER, /**< Generic Pulse Oximeter. */
    PulseOximeterFingertip =
        BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP, /**< Fingertip (Pulse Oximeter subtype). */
    PulseOximeterWristWorn =
        BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN, /**< Wrist Worn(Pulse Oximeter subtype). */
    GenericWeightScale =
        BLE_APPEARANCE_GENERIC_WEIGHT_SCALE, /**< Generic Weight Scale. */
    GenericOutdoorSportsActivity =
        BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT, /**< Generic Outdoor Sports Activity. */
    OutdoorSportsLocationDisplay =
        BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP, /**< Location Display Device (Outdoor Sports Activity subtype). */
    OutdoorSportsLocAndNavigDisp =
        BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP, /**< Location and Navigation Display Device (Outdoor Sports Activity subtype). */
    OutdoorSportsLocationPod =
        BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD, /**< Location Pod (Outdoor Sports Activity subtype). */
    OutdoorSportsLocAndNavigPod =
        BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD /**< Location and Navigation Pod (Outdoor Sports Activity subtype). */
};

enum class DeviceName : uint8_t {
    None = 0xFF,
    Short =
        0x1, /**< Include first 5 letters of name, if possible. If not, include minimally 3. */
    LongestPossible =
        0x2 /**< Assemble adv and fill all remaining bytes with name. */
};

enum class ServiceList : uint8_t {
    None = 0xFF,
    CompleteList16bitUUID =
        BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE /**< Complete list of 16 bit service UUIDs. */
};

enum class ScanFilterPolicy : uint8_t {
    AcceptAllAdvExceptForeignDirected =
        BLE_GAP_SCAN_FP_ACCEPT_ALL, /**< Accept all advertising packets except directed advertising packets not addressed to this device. */

    AcceptWhitelistAdvExceptForeignDirected =
        BLE_GAP_SCAN_FP_WHITELIST, /**< Accept advertising packets from devices in the whitelist except directed packets not addressed to this device. */

    AcceptAllAdvAndUnresolvedDirected =
        BLE_GAP_SCAN_FP_ALL_NOT_RESOLVED_DIRECTED, /**< Accept all advertising packets specified in @ref BLE_GAP_SCAN_FP_ACCEPT_ALL.
                                                        In addition, accept directed advertising packets, where the advertiser's
                                                        address is a resolvable private address that cannot be resolved. */

    AcceptWhitelistAdvAndUnresolvedDirected =
        BLE_GAP_SCAN_FP_WHITELIST_NOT_RESOLVED_DIRECTED /**< Accept all advertising packets specified in @ref BLE_GAP_SCAN_FP_WHITELIST.
                                                        		In addition, accept directed advertising packets, where the advertiser's
                                                            	address is a resolvable private address that cannot be resolved. */
};

/**
 * @brief Enumerates possible values for BLE physical layer data rate.
 */
enum class PhysicalLayer : uint8_t {
    SetAuto  = BLE_GAP_PHY_AUTO, /**< Automatic PHY selection.*/
    Set1MBPS = BLE_GAP_PHY_1MBPS, /**< 1 Mbps PHY. */
    Set2MBPS = BLE_GAP_PHY_2MBPS, /**< 2 Mbps PHY. */
    SetCoded = BLE_GAP_PHY_CODED /**< Coded PHY. */
};

enum class AdvDataField : uint8_t {
    Flags              = 0x01, /**<	«Flags» */
    ShortenedLocalName = 0x08, /**< Shortened Local Name */
    CompleteLocalName  = 0x09, /**< Complete Local Name */
    Appearance         = 0x19, /**< Appearance */
    ManufSpecificData  = 0xFF /**< Manufacturer Specific Data */
};
};  // namespace IO::BLE

#endif  //__AL_BLE_H__
