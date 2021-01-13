/**
 * @file CharacteristicBase.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief base class for all chracteristics
 * @version 1.0
 * @date 2020-08-18
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __CHARACTERISTICBASE_H__
#define __CHARACTERISTICBASE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::BLE
{
class CharacteristicBase;
}

//--------------------------------- INCLUDES ----------------------------------

#include "AL_Service.h"

#include <LifetimeList.h>
#include <sdk_errors.h>

namespace IO::BLE
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief base class for all chracteristics
 */
class CharacteristicBase {
    // delete default constructors
    CharacteristicBase()                                = delete;
    CharacteristicBase(const CharacteristicBase& other) = delete;
    CharacteristicBase& operator=(const CharacteristicBase& other) = delete;

    friend class Utility;

public:
    struct Properties {
        bool broadcast : 1; /**< Broadcasting of the value permitted. */
        bool read : 1;      /**< Reading the value permitted. */
        bool
             writeNoResponse : 1; /**< Writing the value with Write Command permitted. */
        bool write : 1;  /**< Writing the value with Write Request permitted. */
        bool notify : 1; /**< Notification of the value permitted. */
        bool indicate : 1; /**< Indications of the value permitted. */
        bool
            authSignedWrites : 1; /**< Writing the value with Signed Write Command permitted. */

        Properties(bool broadcast        = false,
                   bool read             = true,
                   bool writeNoResponse  = false,
                   bool write            = true,
                   bool notify           = false,
                   bool indicate         = true,
                   bool authSignedWrites = false);
    };

    CharacteristicBase(IO::BLE::Service& parentService,
                       Properties&&      userProperties);

    CharacteristicBase(IO::BLE::Service&              parentService,
                       Properties&&                   userProperties,
                       const std::array<uint8_t, 16>& userBaseUUID,
                       uint16_t                       userCharUUID);

    void                                                   init();
    Service&                                               getService();
    static Collections::LifetimeList<CharacteristicBase&>& getList();
    const Properties&                                      getProperties();

protected:
    void applyCharProperties(ble_gatts_char_md_t* char_md,
                             ble_gatts_attr_md_t* attr_md);
    void checkErrorSoftdeviceHVX(ret_code_t errorCode);
    void transmitValue(bool indicate);

    virtual size_t   getDataSize()    = 0;
    virtual uint8_t* getDataPtr()     = 0;
    virtual void     onValueChanged() = 0;

    static void forwardEvent(uint16_t              connectionHandle,
                             ble_gatts_evt_write_t writeEvent);

    Service& parentService; /**< service this characteristics belongs to */

    uint16_t      charUUID; /**< needed to put together 128Bit uuid */
    ble_uuid128_t baseUUID; /**< base for the 128Bit UUID */
    ble_gatts_char_handles_t
        characteristicHandles; /**< Handles for a single characteristic. This is where softdevice
	                                                        stores handles for our characteristic after it initializes it. */
    Collections::LifetimeList<CharacteristicBase&>::Node
               node; /**< Node which registers every characteristic into characteristicList */
    Properties userProperties; /**< properties of this characteristic */
};
}  // namespace IO::BLE
#endif  //__CHARACTERISTICBASE_H__