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

#ifndef __SYS_UPDATER_H__
#define __SYS_UPDATER_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace SYS
{
class Updater;
}

//--------------------------------- INCLUDES ----------------------------------

namespace SYS
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class Updater {
    // Delete default constructors
    Updater()                     = delete;
    Updater(const Updater& other) = delete;
    Updater& operator=(const Updater& other) = delete;

public:
    static void init();
};

}  // namespace SYS
#endif  //__SYS_UPDATER_H__