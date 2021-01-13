/**
 * @file AL_FlashRecord.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief record in flash
 * @version 1.0
 * @date 2020-03-30
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashRecord.h"

#include "AL_Log.h"
#include "FlashUtility.h"
#include "FunctionScopeTimer.h"

#include <PortUtility.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ScopeExit.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTORS ---------------------------------

/**
 * @brief Create a new record instance.
 * 
 * @tparam Type of the variable in flash
 * 
 * @details The record is a string identified entry
 * in flash of type T.
 * The constructor will neither write nor read the
 * records value. Therefor you will have to read or write it.
 */
template<class T>
constexpr IO::Flash::Record<T>::Record(const char* const identifier, File& file)
        : name(identifier), file(file)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Try to get the value from flash record.
 * 
 * @details Reading from flash is immiate.
 *
 * @param value Variable to write the value to
 * @return Error::Code might not find the record or might be corrupted
 */
template<class T>
Error::Code IO::Flash::Record<T>::tryGet(T& value)
{
    fds_record_desc_t  descriptor = {0};
    fds_flash_record_t record     = {0};

    // try to find the record
    RETURN_ON_ERROR(tryOpen(descriptor, record));
    auto recordCloser = Patterns::make_scopeExit(
        [&descriptor]() { Utility::closeRecord(descriptor); });

    if (record.p_header->length_words < lengthWords()) {
        // the data type is too large to fit into that record
        return Error::TooLarge;
    }

    // all checks succesful, read value
    // do not attempt to read directly, typed values can only be read uint32 -
    // aligned
    memcpy(&value,
           &(static_cast<const char*>(record.p_data)[strlen(name) + 1]),
           sizeof(T));

    recordCloser.deactivate();
    return Utility::closeRecord(descriptor);
}

/**
 * @brief Set the given value in flash.
 * 
 * @warning Will block until done. Refer to class doc.
 * 
 * @warning This is a blocking call.
 * This function has no timeout due to internal implementation.
 * Therefor it might block for quite some time.
 *
 * @param value Value to try to set to.
 * 
 * @return Error::Code Timeout or corrupted entries
 */
template<class T>
Error::Code IO::Flash::Record<T>::trySet(const T& value)
{
    fds_record_desc_t  descriptor = {0};
    fds_flash_record_t record     = {0};

    // Allocate and put together data field
    std::unique_ptr<uint8_t[]> buffer {new uint8_t[lengthBytes()]};
    // copy string plus terminating char
    memcpy(buffer.get(), name, strlen(name) + 1);
    // copy in value
    memcpy(&(buffer.get()[strlen(name) + 1]), &value, sizeof(T));

    // Try to find the record
    auto result = tryOpen(descriptor, record);
    if (result == Error::None) {
        // record already exists, close and update
        RETURN_ON_ERROR(Utility::closeRecord(descriptor));
        return file.updateRecord(descriptor,
                                 record.p_header->record_key,
                                 std::move(buffer),
                                 lengthBytes());
    } else if ((result == Error::NotFound) ||
               (result == Error::ChecksumFailed)) {
        if (result == Error::ChecksumFailed) {
            // first delete then create again
            RETURN_ON_ERROR(file.removeRecord(descriptor));
        }

        // record does not exist, create
        return file.createRecord(getRecordIndex(),
                                 std::move(buffer),
                                 lengthBytes());
    } else {
        return result;
    }
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Calculate crc16 from string identifier.
 * 
 * @tparam T Type of the value in flash
 * @return uint16_t
 */
template<class T>
constexpr uint16_t IO::Flash::Record<T>::getRecordIndex()
{
    uint16_t hash = crc16_compute(reinterpret_cast<const uint8_t*>(name),
                                  strlen(name),
                                  nullptr);
    hash %= (Utility::kRecordKeyMax - Utility::kRecordKeyMin);
    hash += Utility::kRecordKeyMin;
    return hash;
}

/**
 * @brief Length of the data block in flash words rounded up.
 *
 * @tparam T Type of the value in flash
 * @return size_t
 */
template<class T>
constexpr size_t IO::Flash::Record<T>::lengthWords()
{
    return (strlen(name) + 1 + sizeof(T) + Utility::kWordSize - 1) /
           Utility::kWordSize;
}

/**
 * @brief Length of the data block in flash bytes.
 *
 * @tparam T Type of the value in flash
 * @return size_t
 */
template<class T>
constexpr size_t IO::Flash::Record<T>::lengthBytes()
{
    return lengthWords() * Utility::kWordSize;
}

/**
 * @brief Function for opening the record belonging to this FlashRecord instance.
 * 
 * @tparam T Type of the value in flash
 * @param descriptor Outputs the descriptor if found
 * @param record Outputs the record if found.
 * @return Error::Code Might not be found if it does not yet exist in Flash.
 */
template<class T>
Error::Code IO::Flash::Record<T>::tryOpen(fds_record_desc_t&  descriptor,
                                          fds_flash_record_t& record)
{
    descriptor = {0};
    record     = {0};

    for (auto possibleDescriptor : file.findByRecordKey(getRecordIndex())) {
        // open record
        RETURN_ON_ERROR(Utility::openRecord(possibleDescriptor, record));

        // check string identifier
        if (strcmp(name, static_cast<const char*>(record.p_data)) != 0) {
            // this string id does not match
            RETURN_ON_ERROR(Utility::closeRecord(possibleDescriptor));
        } else {
            // this is the right record
            descriptor = possibleDescriptor;
            return Error::None;
        }
    }

    return Error::NotFound;
}

//---------------------------- STATIC FUNCTIONS -------------------------------
