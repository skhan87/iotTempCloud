/**
 * @file AL_Characteristic.cpp
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

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Characteristic.h"

#include <AL_Log.h>
#include <BLE_Utility.h>
#include <Endians.h>
#include <aconnoConfig.h>
#include <memory>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Generic characteristic class constructor.
 * 
 * @details Generates a generic Characteristic which has a default UUID defined
 * 			in aconnoConfig.h, and a generic characteristic UUID.
 * 
 * @param parentService Pointer to the service to which this characteristic
 * 				        belongs.
 */
template <class T>
IO::BLE::Characteristic<T>::Characteristic(Service&     parentService,
                                           Properties&& userProperties,
                                           const T&     userData)
    : CharacteristicBase{parentService, std::move(userProperties)},
      userData(new T(userData))
{
    Endians::machineToBig(*this->userData);
}

/**
 * @brief Custom characteristic class constructor.
 * 
 * @details Generates custom Characteristic which has a user-defined UUID and
 * 			user-defined Service UUID.
 * 
 * @param userBaseUUID Base UUID chosen by user.
 * @param userCharUUID Service UUID chosen by user.
 * @param parentService Pointer to the service to which this characteristic
 * 				        belongs.
 */
template <class T>
IO::BLE::Characteristic<T>::Characteristic(
    IO::BLE::Service&              parentService,
    Properties&&                   userProperties,
    const T&                       userData,
    const std::array<uint8_t, 16>& userBaseUUID,
    uint16_t                       userCharUUID)
    : CharacteristicBase{parentService,
                         std::move(userProperties),
                         userBaseUUID,
                         userCharUUID},
      userData(new T(userData))
{}

template <class T>
void IO::BLE::Characteristic<T>::updateValue(const T& newValue)
{
    *userData = newValue;
    Endians::machineToBig(*userData);

    // Call to softdevice update function while not in connection
    // will cause a crash.
    if (Utility::isConnected()) {
        if (userProperties.indicate) {
            transmitValue(true);
        } else if (userProperties.notify) {
            transmitValue(false);
        }
    }
}

template <class T>
const T IO::BLE::Characteristic<T>::getValue()
{
    Endians::bigToMachine(*userData);
    return (*userData);
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

template <class T>
size_t IO::BLE::Characteristic<T>::getDataSize()
{
    return sizeof(T);
}

template <class T>
uint8_t* IO::BLE::Characteristic<T>::getDataPtr()
{
    return reinterpret_cast<uint8_t*>(userData.get());
}

template <class T>
void IO::BLE::Characteristic<T>::onValueChanged()
{
    T value{*userData};
    Endians::bigToMachine(value);
    this->trigger(value);
}

//---------------------------- STATIC FUNCTIONS -------------------------------
