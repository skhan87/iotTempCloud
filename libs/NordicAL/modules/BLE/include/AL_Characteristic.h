/**
 * @file AL_Characteristic.h
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief Implements aconno BLE Characteristic class.
 * 
 * @details This class implements BLE Characteristic, integral part of 
 * 			GATT profiles and BLE Services.
 * 
 * 			Characteristic class is a template class, with characteristic data
 * 			array size as the template argument. As a template class, it cannot
 * 			be added directly into characteristicList LifetimeList. Due to this,
 * 			abstract BaseCharacteristic class was created, which registers into
 * 			the list. Characteristic class is derived from BaseCharacteristic.
 * 
 * 			Characteristics are always added to existing Services. User can opt 
 * 			to create a generic Characteristic (the "I don't care about details,
 * 			just give me a characteristic" approach), or create a Characteristic
 * 			with custom base and Characteristic UUIDs.
 * 
 * 			Characteristics need to be created outside of any function (main() 
 * 			included) in order to be properly allocated in memory. Creating the
 * 			right before main() works okay.
 * 
 * 			Upon creation, each Characteristic adds itself into a list of all 
 * 			active characteristics, characteristicList, which is a part of 
 * 			BLE_Utility class. This list is available for use as needed.
 * 
 * @version 1.0
 * @date 2020-7-15
 *
 * @copyright aconno GmbH (c) 2020
 */

#ifndef __AL_CHARACTERISTIC_H__
#define __AL_CHARACTERISTIC_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::BLE
{
template <class T>
class Characteristic;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_BLE.h"
#include "CharacteristicBase.h"
#include "Observable.h"

#include <memory>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

template <class T>
class Characteristic :
    public CharacteristicBase,
    public Patterns::Observable<T> {
    // Delete default constructors
    Characteristic()                            = delete;
    Characteristic(const Characteristic& other) = delete;
    Characteristic& operator=(const Characteristic& other) = delete;

public:
    // Class constructors
    Characteristic(Service&     parentService,
                   Properties&& userProperties,
                   const T&     userData);

    Characteristic(Service&                       parentService,
                   Properties&&                   userProperties,
                   const T&                       userData,
                   const std::array<uint8_t, 16>& userBaseUUID,
                   uint16_t                       userCharUUID);

    void updateValue(const T& newValue);

    const T getValue();

private:
    virtual size_t   getDataSize() final;
    virtual uint8_t* getDataPtr() final;
    virtual void     onValueChanged() final;

    /**
     * @brief pointer to heap allocated buffer for sending.
     * 
     * @details Needed to ensure static position in memory during context switches.
     * 
     */
    std::unique_ptr<T> userData;
};
}  // namespace IO::BLE

#include "../src/AL_Characteristic.cpp"

#endif  //__AL_CHARACTERISTIC_H__