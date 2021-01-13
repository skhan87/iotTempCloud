/**
 * @file AL_AdvertisementTimer.cpp
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * @brief Advertisement timer class inside Advertisement
 * @version 1.0
 * @date 2020-06-04
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Advertisement.h"
#include <AL_Log.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Advertisement timer class constructor.
 * 
 * @return  None.
 */
IO::BLE::Advertisement::Timer::Timer(IO::BLE::Advertisement& advertisement,
                                     const char* const       name,
                                     RTOS::milliseconds      timeMs,
                                     bool                    restarting)
    : RTOS::Timer(name, timeMs, restarting), advertisement(advertisement)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
/**
 * @brief Callback function for advertisement timers.
 *
 * @details This function is called when any of the advertisement timers times out.
 * 			It places pointer to advertisement which should be broadcasted into
 * 			the broadcast queue. 
 * 
 * @return  None.
 */
void IO::BLE::Advertisement::Timer::onTimer()
{
    if (advertisement.trigger(10) != Error::None) {
        LOG_W("Advertisement queue is full");
    }
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
