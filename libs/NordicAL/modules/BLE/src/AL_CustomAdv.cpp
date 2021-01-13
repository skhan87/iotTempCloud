/**
 * @file	AL_CustomAdv.cpp
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

//--------------------------------- INCLUDES ----------------------------------
#include "AL_CustomAdv.h"
#include "BLE_Utility.h"
#include "aconnoConfig.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief	Customizable advertisement constructor.
 * 
 * @param	interval	Time between two broadcasts of this advertisement,
 * 						in [ms].
 * @param	txPower 	Power level at which this advertisement shall be 
 * 						broadcasted.
 * @param	companyId	Company ID, used as part of manufacturer-specific 
 * 						data field.
 * @param	manufacturerData	User data to be sent with the advertisement.
 * @param	burstCount	Number of copies of same advertisement to be sent 
 * 						simultaneously. Burst advertisement have higher chances 
 * 						of reaching destination in bluetooth-noisy environments. 
 * @param	autostart	Determines wheather advertisement starts broadcasting 
 * 						automatically at system startup or if it has to be 
 * 						started explicitly in the code.
 * @param	advFlags  	Set flags field option for this advertisement.
 * @param	advAppearance 	Set appearance field option for this advertisement. 
 * @param	advServiceList 	Set service list field option for this advertisement. 
 * @param	advDeviceName 	Set device name field option for this advertisement. 
 * 
 */
template <size_t ManufSpecDataSize>
IO::BLE::CustomAdv<ManufSpecDataSize>::CustomAdv(
    RTOS::milliseconds                       interval,
    TxPower                                  txPower,
    CompanySigId                             companyId,
    std::array<uint8_t, ManufSpecDataSize>&& manufacturerData,
    uint8_t                                  burstCount,
    bool                                     autostart,
    Flags                                    advFlags,
    Appearance                               advAppearance,
    ServiceList                              advServiceList,
    DeviceName                               advDeviceName)
    : Advertisement(interval, txPower, burstCount, autostart),
      companyId{companyId}, manufacturerData{manufacturerData},
      advFlags{advFlags}, advAppearance{advAppearance},
      advServiceList{advServiceList}, advDeviceName{advDeviceName}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------
/**
 * @brief	This function gets triggered at every advertisement interval expiry.
 * 
 * @details	Implements advertisement functionality. This function gets called 
 * 			every time advertisement timer expires,	every 'interval' milliseconds.
 * 
 * @param	timeoutMs Time to wait for async operation
 * 
 * @return	Status of advertisement enqueue into advertising queue attempt.
 */
template <size_t ManufSpecDataSize>
Error::Code IO::BLE::CustomAdv<ManufSpecDataSize>::trigger(
    RTOS::milliseconds timeout)
{
    size_t packageLen = 0;

    // Dry run - calculate size only
    buildPackage(nullptr, packageLen);

    // Allocate memory for advertisement data package
    uint8_t* package = new uint8_t[packageLen];

    // Populate package with data
    buildPackage(package, packageLen);

    return queueForAdvertisement(package, packageLen, timeout);
}

/**
 * @brief	Builds advertisement based on user choice of advertisement fields.
 * 
 * @details	If nullptr is passed for package, performs dry run - only calculates
 * 			advertisement size for purposes of memory allocation and returns
 * 			it inside offset variable.
 * 
 * 			If valid array is passed instead, populates it with advertisement
 * 			data according to user choices.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::buildPackage(uint8_t* package,
                                                         size_t&  offset)
{
    offset = 0;

    includeFlags(package, offset);
    includeAppearance(package, offset);
    includeServices(package, offset);
    includeManufSpecData(package, offset);

    /* Warning : this field must be added last, as it is variable and its size
		depends on the rest of package */
    includeDeviceName(package, offset);
}

/**
 * @brief	Encodes manufacturer-specific data field into advertisement.
 * 
 * @details Handles both calculation of memory size requirements of the
 * 			designated advertisement array field, as well as populating the
 * 			field memory with actual field data.
 * 
 * 			To calculate memory requirements only (for memory allocation
 * 			purposes), pass in nullptr as package array parameter. Size of
 * 			field will then be returned inside offset variable. To put actual 
 * 			field data into array, provide valid array.
 * 
 * @param	package	To perform dry-run, simply pass as nullptr. Otherwise pass
 * 					a pointer to the data array allocated for encoding contents 
 * 					of the advertisement.
 * @param	offset	In dry-run scenario returns size of corresponding data field.
 * 					Otherwise acts as counter variable used to encode and move 
 * 					along byte array of the data package.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::includeManufSpecData(
    uint8_t* package,
    size_t&  offset)
{
    size_t fieldSize = 0;

    if (ManufSpecDataSize > 0) {
        // ManufSpecData field enabled; process it
        fieldSize += ManufSpecDataSize;
        fieldSize += sizeof(CompanySigId);
        fieldSize += AD_DATA_OFFSET;

        if (package == nullptr) {
            // Dry run, don't enqueue data, just add field size to offset variable
            offset += fieldSize;
        } else {
            // Not a dry run, enqueue data
            package[offset++] = static_cast<uint8_t>(fieldSize) - 1;
            package[offset++] = static_cast<uint8_t>(
                Advertisement::AdvType::ManufacturerSpecific);
            package[offset++] = reinterpret_cast<uint8_t*>(&companyId)[0];
            package[offset++] = reinterpret_cast<uint8_t*>(&companyId)[1];
            memcpy(&package[(offset)],
                   manufacturerData.data(),
                   ManufSpecDataSize);
            offset += ManufSpecDataSize;
        }
    }
}

/**
 * @brief	Encodes service list field into advertisement.
 * 
 * @details Handles both calculation of memory size requirements of the
 * 			designated advertisement array field, as well as populating the
 * 			field memory with actual field data.
 * 
 * 			To calculate memory requirements only (for memory allocation
 * 			purposes), pass in nullptr as package array parameter. Size of
 * 			field will then be returned inside offset variable. To put actual 
 * 			field data into array, provide valid array.
 * 
 * @param	package	To perform dry-run, simply pass as nullptr. Otherwise pass
 * 					a pointer to the data array allocated for encoding contents 
 * 					of the advertisement.
 * @param	offset	In dry-run scenario returns size of corresponding data field.
 * 					Otherwise acts as counter variable used to encode and move 
 * 					along byte array of the data package.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::includeServices(uint8_t* package,
                                                            size_t&  offset)
{
    size_t fieldSize = 0;

    if (advServiceList != ServiceList::None) {
        size_t numOfActiveServices = Service::getList().size();

        // Include service list field only if active services exist.
        if (numOfActiveServices > 0) {
            fieldSize =
                (numOfActiveServices * bytesPerServiceUUID16) + AD_DATA_OFFSET;
        }

        if (package == nullptr) {
            // Dry run, don't enqueue data, just add field size to offset variable
            offset += fieldSize;
        } else if (fieldSize != 0) {
            // Not a dry run, enqueue data
            package[offset] =
                (uint8_t)(AD_TYPE_FIELD_SIZE + (Service::getList().size() * 2));
            offset += AD_LENGTH_FIELD_SIZE;
            package[offset] = BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE;
            offset += AD_TYPE_FIELD_SIZE;

            for (auto& service : Service::getList()) {
                offset += uint16_encode((uint16_t)service.getServiceUUID().uuid,
                                        &package[offset]);
            }
        }
    }
}

/**
 * @brief	Encodes flags data field into advertisement.
 * 
 * @details Handles both calculation of memory size requirements of the
 * 			designated advertisement array field, as well as populating the
 * 			field memory with actual field data.
 * 
 * 			To calculate memory requirements only (for memory allocation
 * 			purposes), pass in nullptr as package array parameter. Size of
 * 			field will then be returned inside offset variable. To put actual 
 * 			field data into array, provide valid array.
 * 
 * @param	package	To perform dry-run, simply pass as nullptr. Otherwise pass
 * 					a pointer to the data array allocated for encoding contents 
 * 					of the advertisement.
 * @param	offset	In dry-run scenario returns size of corresponding data field.
 * 					Otherwise acts as counter variable used to encode and move 
 * 					along byte array of the data package.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::includeFlags(uint8_t* package,
                                                         size_t&  offset)
{
    std::size_t fieldSize = 0;

    if (advFlags != Flags::None) {
        fieldSize = AD_TYPE_FLAGS_SIZE;

        if (package == nullptr) {
            // Dry run, don't enqueue data, just add field size to offset variable
            offset += fieldSize;
        } else {
            // Not a dry run, enqueue data
            package[offset] =
                (uint8_t)(AD_TYPE_FIELD_SIZE + AD_TYPE_FLAGS_DATA_SIZE);
            offset += AD_LENGTH_FIELD_SIZE;
            package[offset] = BLE_GAP_AD_TYPE_FLAGS;
            offset += AD_TYPE_FIELD_SIZE;
            package[offset] = (uint8_t)this->advFlags;
            offset += AD_TYPE_FLAGS_DATA_SIZE;
        }
    }
}

/**
 * @brief	Encodes appearance field into advertisement.
 * 
 * @details Handles both calculation of memory size requirements of the
 * 			designated advertisement array field, as well as populating the
 * 			field memory with actual field data.
 * 
 * 			To calculate memory requirements only (for memory allocation
 * 			purposes), pass in nullptr as package array parameter. Size of
 * 			field will then be returned inside offset variable. To put actual 
 * 			field data into array, provide valid array.
 * 
 * @param	package	To perform dry-run, simply pass as nullptr. Otherwise pass
 * 					a pointer to the data array allocated for encoding contents 
 * 					of the advertisement.
 * @param	offset	In dry-run scenario returns size of corresponding data field.
 * 					Otherwise acts as counter variable used to encode and move 
 * 					along byte array of the data package.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::includeAppearance(uint8_t* package,
                                                              size_t&  offset)
{
    std::size_t fieldSize = 0;

    if (advAppearance != Appearance::None) {
        fieldSize = AD_TYPE_APPEARANCE_SIZE;

        if (package == nullptr) {
            // Dry run, don't enqueue data, just add field size to offset variable
            offset += fieldSize;
        } else {
            // Not a dry run, enqueue data
            package[offset] =
                (uint8_t)(AD_TYPE_FIELD_SIZE + AD_TYPE_APPEARANCE_DATA_SIZE);
            offset += AD_LENGTH_FIELD_SIZE;
            package[offset] = BLE_GAP_AD_TYPE_APPEARANCE;
            offset += AD_TYPE_FIELD_SIZE;
            offset +=
                uint16_encode((uint16_t)this->advAppearance, &package[offset]);
        }
    }
}

/**
 * @brief	Encodes device name field into advertisement.
 * 
 * @details Handles both calculation of memory size requirements of the
 * 			designated advertisement array field, as well as populating the
 * 			field memory with actual field data.
 * 
 * 			To calculate memory requirements only (for memory allocation
 * 			purposes), pass in nullptr as package array parameter. Size of
 * 			field will then be returned inside offset variable. To put actual 
 * 			field data into array, provide valid array.
 * 
 * 			This field is always placed last, as it size depends on bytes
 * 			available after all other fields have been placed into advertisement
 * 			array. If even 'minimalDeviceNameSize' name does not fit the
 * 			advertisement, device name field will not be included.
 * 
 * @param	package	To perform dry-run, simply pass as nullptr. Otherwise pass
 * 					a pointer to the data array allocated for encoding contents 
 * 					of the advertisement.
 * @param	offset	In dry-run scenario returns size of corresponding data field.
 * 					Otherwise acts as counter variable used to encode and move 
 * 					along byte array of the data package.
 * 
 * @return	None.
 */
template <size_t ManufSpecDataSize>
void IO::BLE::CustomAdv<ManufSpecDataSize>::includeDeviceName(uint8_t* package,
                                                              size_t&  offset)
{
    size_t  fieldSize = 0;
    uint8_t deviceNameFormatType;
    size_t  lenWithoutDevName = package ? (offset + 1) : (offset);

    if (advDeviceName != DeviceName::None) {
        // Obtain size of device name
        size_t nameSize = strlen((const char*)Config::DEVICE_NAME);

        size_t remainingAdvBytes =
            BLE_GAP_ADV_SET_DATA_SIZE_MAX - lenWithoutDevName - AD_DATA_OFFSET;

        fieldSize += AD_DATA_OFFSET;

        // Figure out which size of device name fits the current advertisement
        if ((advDeviceName == DeviceName::LongestPossible) &&
            (remainingAdvBytes >= nameSize)) {
            // Complete device name can fit, setting Complete Name in Adv Data.
            fieldSize += nameSize;
            deviceNameFormatType = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        } else if ((advDeviceName == DeviceName::Short) &&
                   (remainingAdvBytes >= shortDeviceNameSize)) {
            // Check if full short name fits
            fieldSize += shortDeviceNameSize;
            deviceNameFormatType = BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME;
        } else if (remainingAdvBytes >= minimalDeviceNameSize) {
            // Else fill in what fits (minimum 3 letters)
            fieldSize += remainingAdvBytes;
            deviceNameFormatType = BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME;
        } else {
            // Name does not fit this advertisement, skip it
            fieldSize = 0;
        }

        if (package == nullptr) {
            // Dry run, don't enqueue data, just add field size to offset variable
            offset += fieldSize;
        } else if (fieldSize != 0) {
            // Not a dry run, enqueue data
            size_t sizeWhichFits = fieldSize - AD_DATA_OFFSET;
            package[offset] = (uint8_t)(AD_TYPE_FIELD_SIZE + sizeWhichFits);
            offset += AD_LENGTH_FIELD_SIZE;
            package[offset] = deviceNameFormatType;
            offset += AD_TYPE_FIELD_SIZE;
            memcpy(&package[offset], Config::DEVICE_NAME, sizeWhichFits);
            offset += sizeWhichFits;
        }
    }
}

//---------------------------- STATIC FUNCTIONS -------------------------------