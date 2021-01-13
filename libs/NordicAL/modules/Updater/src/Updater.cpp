/**
 * @file	Updater.h
 * @author	Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief	Manages updating process of aconno firmware.
 * 
 * @details	This update manager at the moment utilizes DFU module to enable user
 * 			to update firmware on an aconno device. In the future this module
 * 			shall blend and combine various ways to update the device, i.e.
 * 			DFU, USB, NFC, etc.
 * 
 * @version	0.1.0
 * @date 	2020-09-18
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------
#include "AL_DFU.h"
#include "Updater.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
void SYS::Updater::init()
{
    DFU::init();
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
