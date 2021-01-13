/**
 * @file AL_Advertisement.h
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * @author Joshua Lauterbach (joshua@aconno.de)
 * 
 * @brief BLE advertisement implementation.
 * 
 * @details This file implements an abstract ble advertisement class. To create
 * 			a new advertisement, inherit from this class and implement your own
 * 			virtual functions.
 * 
 * @version 1.0
 * @date 2020-06-04
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_ADVERTISEMENT_H__
#define __AL_ADVERTISEMENT_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::BLE
{
class Advertisement;
}

//--------------------------------- INCLUDES ----------------------------------
#include "AL_BLE.h"
#include "Advertiser.h"

#include <AL_Timer.h>
#include <Error.h>
#include <ble.h>
#include <ble_advdata.h>
#include <cstdint>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class Advertisement {
private:
    class Timer : public RTOS::Timer {
        // Delete default constructors
        Timer()                   = delete;
        Timer(const Timer& other) = delete;
        Timer& operator=(const Timer& other) = delete;

    public:
        // Class constructors
        Timer(BLE::Advertisement& advertisement,
              const char* const   name,
              RTOS::milliseconds  timeMs,
              bool                restarting);

    private:
        virtual void onTimer() final; /** implements RTOS::Timer */
        BLE::Advertisement&
            advertisement; /**< Pointer storing outer class of timer instance */
    };

public:
    /* Enum used to identify the type of advertisement */
    enum class AdvType : uint8_t {
        ManufacturerSpecific = 0xFF,
    };

    // Delete default constructors
    Advertisement()                           = delete;
    Advertisement(const Advertisement& other) = delete;
    Advertisement& operator=(const Advertisement& other) = delete;

    // Class constructor
    Advertisement(RTOS::milliseconds interval,
                  TxPower            txPower,
                  uint8_t            burstCount = 1,
                  bool               autostart  = true);

    // User API functions
    Error::Code        start(RTOS::milliseconds timeout = RTOS::Infinity);
    Error::Code        stop(RTOS::milliseconds timeout = RTOS::Infinity);
    Error::Code        setInterval(RTOS::milliseconds interval,
                                   RTOS::milliseconds timeoutMs = RTOS::Infinity);
    Error::Code        setIntervalFromISR(RTOS::milliseconds interval,
                                          bool* contextSwitchNeeded = nullptr);
    RTOS::milliseconds getInterval();
    Error::Code        setTXPower(TxPower txPower);
    TxPower            getTXPower();
    Error::Code        setBurstCount(uint8_t count);
    uint8_t            getBurstCount();

    /*--- Public constants ---*/
    /**
	 * Minimum time period between two advertisements mandated by BT specification.
	 * In milliseconds.
	 */
    static constexpr RTOS::milliseconds MIN_INTERVAL = 20;

    /**
	 * Maximum time period between two broadcasts of the same advertisement.
	 * This is based on Bluetooth specification rule that maximal allow time period 
	 * between two instances of same advertisement is 10.24 seconds.
	 * This limit is set lower to allow for BLE protocol transport layer randomized 
	 * advertisement delays.
	 * In milliseconds.
     * 
     *  @warning ignored to enable higher advertisement intervals
	 */
    static constexpr RTOS::milliseconds MAX_INTERVAL = 10001;

    virtual Error::Code
        trigger(RTOS::milliseconds timeoutMs = RTOS::Infinity) = 0;

protected:
    Error::Code
        queueForAdvertisement(uint8_t*           data,
                              size_t             len,
                              RTOS::milliseconds timeout = RTOS::Infinity);

private:
    static constexpr bool validInterval(RTOS::milliseconds valueToVerify);
    static constexpr bool validAdvDataSize(uint8_t valueToVerify);

    Timer advBroadcastTimer; /**< Instance of timer which triggers adv. broacasting */
    uint8_t burstCount; /**< number of bursts send per trigger */
    TxPower txPower; /**< Advertisement broadcast radio TX power level. */

    /*--- Private constants ---*/
    /**
	 * Size of aconno adv. header in bytes. 
	 * Does not include BLE field protocol bytes.
	 */
    static constexpr uint8_t ACONNO_ADV_HEADER_BYTES = 5;

    /**
	 * Total number of adv. data bytes reserved by aconno adv. header. 
	 * Includes BLE protocol field LEN and TYPE bytes.
	 * Complete aconno header: 
	 * {LEN[1B],TYPE[1B],NORDIC_ID[2B],ACONNO_ID[1B],PRODUCT_ID[1B],STGS+VER[1B]}
	 */
    static constexpr uint8_t ACONNO_ADV_HEADER_SIZE =
        ACONNO_ADV_HEADER_BYTES + 2;

    /**
	 * As per Bluetooth standard, BLE advertisement can consist of no more 
	 * than 31 data bytes.
	 */
    static constexpr uint8_t MAX_DATA_BYTES = 31;

    /**
	 * Burst count is limited to reasonable value to remove the need for very
     * large timeout values. Single advertisement takes about 25ms, therefore
     * for larger burst counts firmware would block for seconds to confirm
     * wheather timeout occured or normal operation is in process.
	 */
    static constexpr uint8_t MAX_BURST_COUNT = 20;

    /**
	 * BLE flag field reserves 3 bytes out of 31 adv. bytes.
	 */
    static constexpr uint8_t ADV_FLAG_FIELD_SIZE = 3;

    /**
	 * Each BLE field has mandatory SIZE and TYPE bytes, 
	 * reserving 2 bytes out of 31 adv. bytes.
	 */
    static constexpr uint8_t ADV_USER_DATA_FIELD_BYTES = 2;

    /**
	 * Maximum number of advertisement data bytes available to user.
	 */
    static constexpr uint8_t ADV_MAX_SIZE =
        MAX_DATA_BYTES - ADV_FLAG_FIELD_SIZE - ADV_USER_DATA_FIELD_BYTES;
};

}  // namespace IO::BLE

#endif  //__AL_ADVERTISEMENT_H__
