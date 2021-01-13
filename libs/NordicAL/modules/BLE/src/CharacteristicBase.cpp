/**
 * @file CharacteristicBase.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief base class for BLE Characteristics
 * @version 1.0
 * @date 2020-08-18
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "CharacteristicBase.h"

#include <AL_Log.h>
#include <Endians.h>
#include <PortUtility.h>
#include <cstring>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

IO::BLE::CharacteristicBase::CharacteristicBase(IO::BLE::Service& parentService,
                                                Properties&& userProperties)
    : CharacteristicBase(parentService,
                         std::move(userProperties),
                         Config::baseUUID,
                         ++Utility::uuidCount)
{}

IO::BLE::CharacteristicBase::CharacteristicBase(
    IO::BLE::Service&              parentService,
    Properties&&                   userProperties,
    const std::array<uint8_t, 16>& userBaseUUID,
    uint16_t                       userCharUUID)
    : parentService(parentService), charUUID(userCharUUID),
      node(getList().appendStatic(*this)), userProperties(userProperties)
{
    auto bigEndianUUID{userBaseUUID};
    std::reverse(bigEndianUUID.begin(), bigEndianUUID.end());
    Endians::machineToBig(bigEndianUUID);
    for (size_t i = 0; i < bigEndianUUID.size(); i++) {
        this->baseUUID.uuid128[i] = bigEndianUUID[i];
    }
}

IO::BLE::CharacteristicBase::Properties::Properties(bool broadcast,
                                                    bool read,
                                                    bool writeNoResponse,
                                                    bool write,
                                                    bool notify,
                                                    bool indicate,
                                                    bool authSignedWrites)
    : broadcast(broadcast), read(read), writeNoResponse(writeNoResponse),
      write(write), notify(notify), indicate(indicate),
      authSignedWrites(authSignedWrites)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief needed to init after soft device booted
 * 
 */
void IO::BLE::CharacteristicBase::init()
{
    // Add a custom characteristic UUID
    ble_uuid_t char_uuid;
    char_uuid.uuid = this->charUUID;
    auto errCode   = sd_ble_uuid_vs_add(&this->baseUUID, &char_uuid.type);
    CHECK_ERROR(Port::Utility::getError(errCode));

    // Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));

    /* Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
	 * CCCD is a writable descriptor that allows the client, i.e. your nRF Connect for Desktop or phone,
	 * to enable or disable characteristic notifications or indications. */
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));

    /* Preparing CCCD to allow notifications enabling by client.
	 * CCCD has its own security, encryption, passkeys options. turning them off. */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(
        &cccd_md.read_perm);  // disable security, encryption, passkey for R/W
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc =
        BLE_GATTS_VLOC_STACK;  // decide where to store CCCD in memory
    char_md.p_cccd_md = &cccd_md;

    /* Configure the attribute metadata.
	 * Decide where in memory to store the attributes.
     * We put it in our managed value pointer. */
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_USER;
    // We could also define permissions with authorization requirements here.

    applyCharProperties(&char_md, &attr_md);

    // Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;

    // Set characteristic length in number of bytes
    attr_char_value.max_len  = getDataSize();
    attr_char_value.init_len = getDataSize();
    attr_char_value.p_value  = getDataPtr();

    // Add our new characteristic to the service
    errCode = sd_ble_gatts_characteristic_add(parentService.getServiceHandle(),
                                              &char_md,
                                              &attr_char_value,
                                              &characteristicHandles);
    CHECK_ERROR(Port::Utility::getError(errCode));
}

/**
 * @brief get the parent service
 * 
 * @return Service& 
 */
IO::BLE::Service& IO::BLE::CharacteristicBase::getService()
{
    return parentService;
}

/**
 * @brief get the read-only characteristic properties
 * 
 * @return const Properties&
 */
const IO::BLE::CharacteristicBase::Properties& IO::BLE::CharacteristicBase::
    getProperties()
{
    return userProperties;
}

/**
 * @brief
 *
 * @param
 * 
 * @return None.
 */
void IO::BLE::CharacteristicBase::applyCharProperties(
    ble_gatts_char_md_t* char_md,
    ble_gatts_attr_md_t* attr_md)
{
    if (userProperties.broadcast) {
        // adding char properties IN CHAR DECLARATION.
        // (these are just the guidelines, not actual permissions)
        char_md->char_props.broadcast = 1;
    }

    if (userProperties.read) {
        char_md->char_props.read = 1;
        // Set read security levels to our characteristic
        // Tells softdevice to allow everything; now we can read char.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(
            &attr_md
                 ->read_perm);  // disable security, encryption, passkey for R/W
    }

    if (userProperties.write) {
        char_md->char_props.write = 1;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md->write_perm);
    }

    // If both write types selected, enable only more restrictive and warn
    if ((userProperties.write) && (userProperties.writeNoResponse)) {
        LOG_W("Conflicting characteristic write properies selected!");
        LOG_W("Enabling only write with response.");
        userProperties.writeNoResponse = false;
    } else if (userProperties.writeNoResponse) {
        char_md->char_props.write_wo_resp = 1;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md->write_perm);
    }

    if (userProperties.indicate) {
        char_md->char_props.indicate = 1;
    }

    // If both alert types selected, enable only more restrictive and warn
    if ((userProperties.indicate) && (userProperties.notify)) {
        LOG_W("Conflicting characteristic alert properies selected!");
        LOG_W("Enabling only indications.");
        userProperties.notify = false;
    } else if (userProperties.notify) {
        char_md->char_props.notify = 1;
    }

    if (userProperties.authSignedWrites) {
        char_md->char_props.auth_signed_wr = 1;
    }
}

/**
 * @brief
 *
 * @param
 * 
 * @return None.
 */
void IO::BLE::CharacteristicBase::checkErrorSoftdeviceHVX(ret_code_t errorCode)
{
    if (errorCode != NRF_SUCCESS) {
        switch (errorCode) {
            case NRF_ERROR_BUSY:
                /* Only one indication procedure can be ongoing per connection at a time.
    	           Wait for a BLE_GATTS_EVT_HVC event and retry. */
                LOG_W(
                    "Indication failed - other indication still in progress.");
                break;

            case NRF_ERROR_RESOURCES:
                /* Queue size can be increased by using ble_gatts_conn_cfg_t::hvn_tx_queue_size.
				   Default size is 1.
				   Wait for a BLE_GATTS_EVT_HVN_TX_COMPLETE event and retry. */
                LOG_W("Notify failed - notification queue full.");
                break;

            case BLE_ERROR_INVALID_CONN_HANDLE:
            case NRF_ERROR_INVALID_ADDR:
            case NRF_ERROR_INVALID_PARAM:
            case BLE_ERROR_INVALID_ATTR_HANDLE:
            case BLE_ERROR_GATTS_INVALID_ATTR_TYPE:
            case NRF_ERROR_DATA_SIZE:
                CHECK_ERROR(Error::InvalidParameter);
                break;

            case NRF_ERROR_INVALID_STATE:
                /* One or more of the following is true:
					- Invalid Connection State
					- Notifications and/or indications not enabled in the CCCD
					- An ATT_MTU exchange is ongoing */
                LOG_E("Indication/notification failed - did client enable them "
                      "in CCCD?");
                break;

            case NRF_ERROR_NOT_FOUND:
                CHECK_ERROR(Error::NotFound);
                break;

            case NRF_ERROR_FORBIDDEN:
                LOG_E("Indication/notification failed - write permission too "
                      "low.");
                break;

            case BLE_ERROR_GATTS_SYS_ATTR_MISSING:
                /* System attributes missing, use sd_ble_gatts_sys_attr_set to set them to a known value.
				Per docs: 
					This call is only allowed for active connections, and is usually
					made immediately after a connection is established with an known bonded device,
					often as a response to a @ref BLE_GATTS_EVT_SYS_ATTR_MISSING.
					Therefore, this shall be handled in Utility BLE Event handler. */
                break;

            case NRF_ERROR_TIMEOUT:
                CHECK_ERROR(Error::Timeout);
                break;

            default:
                break;
        }
    }
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief transmit value when notification or indication is used
 * 
 * @param indicate 
 */
void IO::BLE::CharacteristicBase::transmitValue(bool indicate)
{
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));
    uint16_t size = getDataSize();

    hvx_params.handle = this->characteristicHandles
                            .value_handle;  // which char. are we working on

    if (indicate) {
        hvx_params.type =
            BLE_GATT_HVX_INDICATION;  // specify we are sending nofitication
    } else {
        hvx_params.type =
            BLE_GATT_HVX_NOTIFICATION;  // specify we are sending indication
    }

    hvx_params.offset =
        0;  // used if not updating entire characteristic but only a part
    hvx_params.p_len =
        &size;  // how many bytes to send. don't send 20B for a 20B char. if only changing a single B. use offset and len
    hvx_params.p_data =
        reinterpret_cast<uint8_t*>(getDataPtr());  // pointer to actual data

    ret_code_t err_code = sd_ble_gatts_hvx(
        Utility::devConnHandle,
        &hvx_params);  // hvx = handle value x (x = notif or indic)
    checkErrorSoftdeviceHVX(err_code);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief list of all characteristics
 * 
 * @details uses eager loading to keep the right order of construction.
 * This way statically constructed objects always get
 * a valid LifetimeList instance.
 * 
 * @return Collections::LifetimeList<BaseCharacteristic&>& 
 */
Collections::LifetimeList<IO::BLE::CharacteristicBase&>& IO::BLE::
    CharacteristicBase::getList()
{
    static Collections::LifetimeList<CharacteristicBase&> list{};
    return list;
}

/**
 * @brief forwards SD events to the right characteristics.
 * 
 * @param connectionHandle 
 * @param writeEvent 
 */
void IO::BLE::CharacteristicBase::forwardEvent(uint16_t connectionHandle,
                                               ble_gatts_evt_write_t writeEvent)
{
    auto attributeHandle = writeEvent.handle;
    auto attributeUUID   = writeEvent.uuid.uuid;
    auto operation       = writeEvent.op;

    if (operation != BLE_GATTS_OP_WRITE_REQ) {
        // unknown operation
        LOG_W("unknown operation on writeEvent");
        return;
    }

    for (auto& characteristic : getList()) {
        if ((characteristic.charUUID == attributeUUID) &&
            (characteristic.characteristicHandles.value_handle ==
             attributeHandle)) {
            characteristic.onValueChanged();
            break;
        }
    }
}