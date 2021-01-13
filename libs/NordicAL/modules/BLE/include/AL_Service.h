/**
 * @file AL_Service.h
 * @author Hrvoje Brezak (hrvoje@aconno.de)
 * 
 * @brief Implements aconno BLE Service class.
 * 
 * @details This class implements BLE Service, integral part of GATT profile.
 * 			User can opt to create a generic Service (the "I don't care about
 * 			details, just give me a service" approach), or create a Service with
 * 			custom base and Service UUIDs.
 * 
 * 			Upon creation, each Service adds itself into a list of all active
 * 			services, serviceList, which is a part of BLE_Utility class. This
 * 			list is available for use as needed.
 * 
 * @warning	Services need to be created outside of any function (main() included)
 * 			in order to be properly allocated in memory. Creating the right
 * 			before main() works okay. That is also the right place to add
 * 			Characteristics to your new Service.
 * 
 * @version 1.0
 * @date 2020-7-15
 *
 * @copyright aconno GmbH (c) 2020
 */

#ifndef __AL_SERVICE_H__
#define __AL_SERVICE_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace IO::BLE
{
class Service;
}

//--------------------------------- INCLUDES ----------------------------------
#include "ble_srv_common.h"

#include <LifetimeList.h>
#include <array>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class Service {
    // Delete default constructors
    Service(const Service& other) = delete;
    Service& operator=(const Service& other) = delete;

    friend class Utility;

public:
    enum class DisconnectReason : uint8_t {
        AuthentificationFail   = BLE_HCI_AUTHENTICATION_FAILURE,
        MemoryCapacityExceeded = BLE_HCI_MEMORY_CAPACITY_EXCEEDED,
        ConnectionTimeout      = BLE_HCI_CONNECTION_TIMEOUT,
        TerminatedByRemote     = BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION,
        TerminatedByHost       = BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION,
        RemoteLowResources =
            BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES,
        RemotePowerDown = BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF
    };

    using DisconnectHandlerT = void (*)(DisconnectReason reason);

    /*--- Constructors ---*/
    Service();
    Service(const std::array<uint8_t, 16>& userBaseUUID,
            uint16_t                       userServiceUUID);

    /*--- Public API ---*/
    static Collections::LifetimeList<Service&>& getList();
    static Error::Code   setDisconnectHandler(DisconnectHandlerT handler);
    const uint16_t&      getServiceHandle();
    const ble_uuid_t&    getServiceUUID();
    const ble_uuid128_t& getServiceBaseUUID();

private:
    /*--- Private API ---*/
    void init();

    /*--- Private members ---*/
    ble_uuid_t    serviceUUID;
    ble_uuid128_t baseUUID;
    uint16_t
        serviceHandle; /**< Handle of Our Service (as provided by the BLE stack). */

    Collections::LifetimeList<Service&>::Node
        node; /**< Node which registers each service into serviceList */

    static DisconnectHandlerT disconnectHandler;
    static void               onDisconnect(uint8_t reason);
    /**
     * @brief transforms the disconnect reason to official type
     * 
     * @param reason 
     * @return constexpr DisconnectReason 
     */
    static constexpr DisconnectReason getDisconnectReason(uint8_t reason)
    {
        return static_cast<DisconnectReason>(reason);
    }
};

}  // namespace IO::BLE

#endif  //__AL_SERVICE_H__
