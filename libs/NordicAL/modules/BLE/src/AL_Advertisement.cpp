/**
 * @file AL_Advertisement.cpp
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

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Advertisement.h"

#include "Advertiser.h"
#include <Error.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Advertisement class main constructor.
 * 
 * @details Uses data provided by user to set private class variables.
 * 
 * @param	interval	Period of broadcasting the advertisement, in milliseconds.
 * @param	txPower		Transmission power of the advertisement, in decibels.
 * @param   burstCount  Number of adv packages send per triggererd burst. Must not be 0.
 * @param   autostart   If set to true, advertisement timer is started automaticly.
 * 
 * @warning Burst count must not be 0, as this would cause endless advertising on the
 *          same advertisement.
 */
IO::BLE::Advertisement::Advertisement(RTOS::milliseconds interval,
                                      TxPower            txPower,
                                      uint8_t            burstCount,
                                      bool               autostart)
        : advBroadcastTimer(*this, "advTimer", interval, true),
          burstCount(burstCount), txPower(txPower)
{
    // Perform user input argument sanity checks
    if (!validInterval(interval)) {
        CHECK_ERROR(Error::InvalidUse);
    }

    if ((burstCount == 0) || (burstCount > MAX_BURST_COUNT)) {
        CHECK_ERROR(Error::InvalidParameter);
    }

    if (autostart) {
        CHECK_ERROR(start());
    }
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Initializes and starts the advertisement.
 * 
 * @return 	None.
 */
Error::Code IO::BLE::Advertisement::start(RTOS::milliseconds timeout)
{
    /* Once advertisement module is up and running, start advertisements 
	 * simply by starting their advertisement timer.
	 */
    return advBroadcastTimer.start(timeout);
}

/**
 * @brief Stops the advertisement broadcasting.
 * 
 * @details Advertisement broadcasts are triggered by timers. If timer is stopped,
 * 			advertisement will not be put into advertisement queue anymore.
 * 			If all advertisements are stopped, BLE advertisement will remain
 * 			in turned off state after last radio notification is received.
 * 			Since advertising module remains initialized, start method can be
 * 			used to resume advertisement broadcasts.
 * 
 * @return 	None.
 */
Error::Code IO::BLE::Advertisement::stop(RTOS::milliseconds timeout)
{
    return advBroadcastTimer.stop(timeout);
}

/**
 * @brief Advertisement interval setter function.
 * 
 * @details Performs sanity check on given user input and stores it in private
 * 			class member if valid. Do not use from ISR!
 * 
 * @param interval Period of broadcasting the advertisement, in milliseconds.
 * @param timeoutMs time to wait async
 * 
 * @return Error::Code None if successful, Invalid use or Async error otherwise
 */
Error::Code IO::BLE::Advertisement::setInterval(RTOS::milliseconds interval,
                                                RTOS::milliseconds timeoutMs)
{
    if (!validInterval(interval)) {
        return Error::InvalidUse;
    }

    return advBroadcastTimer.setTotalTimeMs(interval, timeoutMs);
}

/**
 * @brief Advertisement interval setter function for calling from interrupt service routine.
 * 
 * @details Performs sanity check on given user input and stores it in private
 * 			class member if valid.
 * 
 * @param interval Period of broadcasting the advertisement, in milliseconds.
 * @param contextSwitchNeeded whether application dev must invoke the scheduler from ISR.
 * 
 * @return Error::Code None if successful, Invalid use or Async error otherwise
 */
Error::Code
    IO::BLE::Advertisement::setIntervalFromISR(RTOS::milliseconds interval,
                                               bool* contextSwitchNeeded)
{
    if (!validInterval(interval)) {
        if (contextSwitchNeeded) {
            *contextSwitchNeeded = false;
        }
        return Error::InvalidUse;
    }

    return advBroadcastTimer.setTotalTimeMsFromISR(interval,
                                                   contextSwitchNeeded);
}

/**
 * @brief sets number of packages sent per trigger.
 * 
 * @param count 
 * 
 * @return Error::Code 
 */
Error::Code IO::BLE::Advertisement::setBurstCount(uint8_t count)
{
    if ((count == 0) || (count > MAX_BURST_COUNT)) {
        return Error::InvalidParameter;
    } else {
        burstCount = count;
        return Error::None;
    }
}

/**
 * @brief gets the number of packages sent per trigger.
 * 
 * @return uint8_t 
 */
uint8_t IO::BLE::Advertisement::getBurstCount()
{
    return burstCount;
}

/**
 * @brief Advertisement TX power setter function.
 * 
 * @details Performs sanity check on given user input and stores it in private
 * 			class member if valid.
 * 
 * @param	txPower	Transmission power of the advertisement, in decibels.
 * @return	true	Value set sucessfully.
 * @return	false	Invalid value.
 */
Error::Code IO::BLE::Advertisement::setTXPower(TxPower txPower)
{
    this->txPower = txPower;
    return Error::None;
}

/**
 * @brief Advertisement class getter method for advertisement interval.
 * 
 * @return	milliseconds	Returns adv. interval value in milliseconds.
 */
RTOS::milliseconds IO::BLE::Advertisement::getInterval()
{
    return advBroadcastTimer.getTotalTimeMs();
}

/**
 * @brief Advertisement class getter method for TX power member.
 * 
 * @return	TxPower	Returns TxPower value.
 */
IO::BLE::TxPower IO::BLE::Advertisement::getTXPower()
{
    return this->txPower;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
/**
 * @brief puts data into the advertisement queue.
 * 
 * @warning do not use from ISR
 * 
 * @param data heap allocated data array. Will be freed by Advertiser.
 * @param len length of heap alloc data array
 * @param timeout 
 * 
 * @return Error::Code 
 */
Error::Code
    IO::BLE::Advertisement::queueForAdvertisement(uint8_t*           data,
                                                  size_t             len,
                                                  RTOS::milliseconds timeout)
{
    Error::Code retVal = Advertiser::getInstance().advertisementQueue.send(
        {txPower, burstCount, data, len},
        timeout);

    // If enqueue failed, we have to free memory here.
    if (retVal != Error::None) {
        delete (data);
    }

    return retVal;
}

/**
 * @brief Advertisement interval value sanity checker.
 * 
 * @param	valueToVerify	Time duration in milliseconds. 
 * @return	true			Value is valid.
 * @return	false			Value is invalid.
 */
bool constexpr IO::BLE::Advertisement::validInterval(
    RTOS::milliseconds valueToVerify)
{
    return ((valueToVerify >= IO::BLE::Advertisement::MIN_INTERVAL) &&
            (valueToVerify < IO::BLE::Advertisement::MAX_INTERVAL));
}

/**
 * @brief Advertisement data size value sanity checker.
 * 
 * @param	valueToVerify	Number of bytes of data to be advertised.
 * @return	true			Value is valid.
 * @return	false			Value is invalid.
 */
bool constexpr IO::BLE::Advertisement::validAdvDataSize(uint8_t valueToVerify)
{
    return ((valueToVerify > 0) &&
            (valueToVerify < IO::BLE::Advertisement::ADV_MAX_SIZE));
}
