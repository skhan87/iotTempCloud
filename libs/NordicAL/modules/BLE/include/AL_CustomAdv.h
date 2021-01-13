/**
 * @file	AL_CustomAdv.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief 	Implementation of fully customizable user advertisement.
 * 
 * @details	This class provides user with possibilty of creating an 
 * 			advertisement containing any number of standard bluetooth low energy
 * 			advertisement fields. Choice of which fields to include and in which
 * 			form is determined by user through options provided to the class
 * 			constructor at time of instantiation.
 * 
 * 			Use example
 * 			-----------
 * 			static IO::BLE::CustomAdv <3> 
 * 			testAdv(1000,
 *					IO::BLE::TxPower::p0dB,
 *					IO::BLE::CompanySigId::NordicSemiconductor,
 *					{0xAA, 0xBB, 0xCC},
 *					10,
 *					true,
 *					IO::BLE::Flags::GeneralDiscoverableModeWithBLEOnly,
 *					IO::BLE::Appearance::Unknown,
 *					IO::BLE::ServiceList::CompleteList16bitUUID,
 *					IO::BLE::DeviceName::LongestPossible);

 * @warning	In order to properly allocate advertisement objects in memory always 
 * 			either create them with 'static' keyword or otherwise create them 
 * 			outside of any function (i.e. before main() ).
 * 
 * @version 0.1.0
 * @date 	2020-08-11
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_CUSTOMADV_H__
#define __AL_CUSTOMADV_H__

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Advertisement.h"
#include "AL_BLE.h"

#include <cstdlib>

//-------------------------------- PROTOTYPES ---------------------------------
namespace IO::BLE
{
template <std::size_t ManufSpecDataSize>
class CustomAdv;
}

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
template <std::size_t ManufSpecDataSize>
class CustomAdv : public Advertisement {
    // Delete default constructors
    CustomAdv()                       = delete;
    CustomAdv(const CustomAdv& other) = delete;
    CustomAdv& operator=(const CustomAdv& other) = delete;

public:
    /* --- Constructor --- */
    CustomAdv(RTOS::milliseconds                       interval,
              TxPower                                  txPower,
              CompanySigId                             companyId,
              std::array<uint8_t, ManufSpecDataSize>&& manufacturerData,
              uint8_t                                  burstCount = 1,
              bool                                     autostart  = true,
              Flags                                    advFlags   = Flags::None,
              Appearance  advAppearance  = Appearance::None,
              ServiceList advServiceList = ServiceList::None,
              DeviceName  advDeviceName  = DeviceName::None);

private:
    /* --- Private API --- */
    void                buildPackage(uint8_t* package, size_t& offset);
    void                includeFlags(uint8_t* package, size_t& offset);
    void                includeAppearance(uint8_t* package, size_t& offset);
    void                includeManufSpecData(uint8_t* package, size_t& offset);
    void                includeDeviceName(uint8_t* package, size_t& offset);
    void                includeServices(uint8_t* package, size_t& offset);
    virtual Error::Code trigger(RTOS::milliseconds timeout = RTOS::Infinity)
        final; /**< implements Advertisement functionality */

    /* --- Private members --- */
    uint16_t companyId; /**< manufacturer id put into the advertisement */
    std::array<uint8_t, ManufSpecDataSize>
                manufacturerData; /**< manufacturer data put in the manufacturer specific data area */
    Flags       advFlags;
    Appearance  advAppearance;
    ServiceList advServiceList;
    DeviceName  advDeviceName;

    /* --- Private constants --- */
    static constexpr uint8_t minimalDeviceNameSize = 3;
    static constexpr uint8_t shortDeviceNameSize   = 5;
    static constexpr uint8_t bytesPerServiceUUID16 = 2;
};

}  // namespace IO::BLE

#include "../src/AL_CustomAdv.cpp"
#endif  //__AL_CUSTOMADV_H__