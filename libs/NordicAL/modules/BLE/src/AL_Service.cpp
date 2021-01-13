/**
 * @file AL_Service.cpp
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief Implements aconno BLE Service class.
 * 
 * @details This class implements BLE Service, integral part of GATT profile.
 * 			User can opt to create a generic Service (the "I don't care about
 * 			details, just give me a service" approach), or create a Service with
 * 			custom base and Service UUIDs.
 * 
 * 			Services need to be created outside of any function (main() included)
 * 			in order to be properly allocated in memory. Creating the right
 * 			before main() works okay. That is also the right place to add
 * 			Characteristics to your new Service.
 * 
 * 			Upon creation, each Service adds itself into a list of all active
 * 			services, serviceList, which is a part of BLE_Utility class. This
 * 			list is available for use as needed.
 * 
 * @version 1.0
 * @date 2020-7-15
 *
 * @copyright aconno GmbH (c) 2020
 */

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Service.h"

#include "BLE_Utility.h"
#include "Error.h"

#include <AL_Log.h>
#include <Endians.h>
#include <PortUtility.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

IO::BLE::Service::DisconnectHandlerT IO::BLE::Service::disconnectHandler =
    nullptr;

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Generic user Service class constructor.
 * 
 * @details Generates Service with default base UUID (found in aconnoConfig.h)
 * 			and automatically incrementing index.
 */
IO::BLE::Service::Service() : Service(Config::baseUUID, ++Utility::uuidCount) {}

/**
 * @brief Customizable user Service class constructor.
 * 
 * @param userBaseUUID Pointer to base part of UUID, which has to be used if
 *                     custom advertisement is wanted.
 * @param userServiceUUID Portion of UUID unique to this service.
 */
IO::BLE::Service::Service(const std::array<uint8_t, 16>& userBaseUUID,
                          uint16_t                       userServiceUUID)
    : serviceUUID({.uuid = userServiceUUID}),
      node(getList().appendStatic(*this))
{
    std::array<uint8_t, 16> bigEndianUUID{userBaseUUID};
    std::reverse(bigEndianUUID.begin(), bigEndianUUID.end());
    Endians::machineToBig(bigEndianUUID);
    for (size_t i = 0; i < bigEndianUUID.size(); i++) {
        this->baseUUID.uuid128[i] = bigEndianUUID[i];
    }
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
const uint16_t& IO::BLE::Service::getServiceHandle()
{
    return this->serviceHandle;
}

const ble_uuid_t& IO::BLE::Service::getServiceUUID()
{
    return this->serviceUUID;
}

const ble_uuid128_t& IO::BLE::Service::getServiceBaseUUID()
{
    return this->baseUUID;
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------
/**
 * @brief Service initialization function.
 * 
 * @details Registers user custom service with SoftDevice stack
 * 		    and starts the default advertisement to ensure device is
 * 		    discoverable and connectable.
 * 
 * @return None.
 */
void IO::BLE::Service::init()
{
    ret_code_t err_code;

    // Add service UUID to BLE stack table
    err_code = sd_ble_uuid_vs_add(&this->baseUUID, &this->serviceUUID.type);
    CHECK_ERROR(Port::Utility::getError(err_code));

    // Add new service in softdevice
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &this->serviceUUID,
                                        &this->serviceHandle);
    CHECK_ERROR(Port::Utility::getError(err_code));
}

//---------------------------- STATIC FUNCTIONS -------------------------------
/**
 * @brief list of all Service instances.
 * 
 * @details uses eager loading to keep the right order of construction.
 * This way statically constructed objects always get
 * a valid LifetimeList instance.
 * 
 * @return Collections::LifetimeList<Service&>& 
 */
Collections::LifetimeList<IO::BLE::Service&>& IO::BLE::Service::getList()
{
    static Collections::LifetimeList<Service&> list{};
    return list;
}

/**
 * @brief Register a handler that will get called whenever BLE disconnects.
 * 
 * @details this can be used to trigger actions on disconnect or to reset
 * application specific peripherals.
 * 
 * @param handler 
 * @return Error::Code 
 */
Error::Code IO::BLE::Service::setDisconnectHandler(DisconnectHandlerT handler)
{
    if (disconnectHandler != nullptr) {
        return Error::AlreadyInit;
    }

    disconnectHandler = handler;
    return Error::None;
}

/**
 * @brief Forwards disconnection event to disconnect handler.
 * 
 * @details Use setDisconnectHandler to use the disconnect event.
 * 
 * @param reason 
 */
void IO::BLE::Service::onDisconnect(uint8_t reason)
{
    if (disconnectHandler != nullptr) {
        disconnectHandler(getDisconnectReason(reason));
    }
}