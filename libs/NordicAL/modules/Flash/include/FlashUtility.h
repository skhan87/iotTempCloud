/**
 * @file FlashUtility.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief utility functions for nordic sdk flash implementation.
 * 
 * @warning only to be used from within flash library
 * @version 1.0
 * @date 2020-05-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __FLASHUTILITY_H__
#define __FLASHUTILITY_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::Flash
{
class Utility;
}

//--------------------------------- INCLUDES ----------------------------------

#include "Error.h"
#include "fds.h"
#include "crc16.h"

#include <cstdint>

namespace IO::Flash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief utility functions for nordic sdk flash implementation.
 * 
 * @warning only to be used from within flash library
 */
class Utility {
    // allow internal classes to use utilities
    template<class T>
    friend class Record;

    template<class T>
    friend class Collection;

    friend class File;

    // delete default constructors
    Utility()                     = delete;
    Utility(const Utility& other) = delete;
    Utility& operator=(const Utility& other) = delete;

public:
    static void init();
    static void printStats();

private:
    static constexpr uint16_t kRecordKeyMin =
        0x0002; /**< lowest key value possible for records */
    static constexpr uint16_t kRecordKeyMax =
        0xBFFF; /**< highest key value possible for records */
    static constexpr uint16_t kRecordKeyDescriptor =
        0x0001; /**< record id used for file descriptors */
    static constexpr uint16_t kRecordKeyReserved =
        0x0000; /**< record key used for invalid entries. Also reserved by nordic lib. */
    static constexpr uint16_t kFileIdMin =
        0x0000; /**< lowest id possible for files */
    static constexpr uint16_t kFileIdMax =
        0xBFFF; /**< highest id possible for files */
    static constexpr uint16_t kFileIdInvalid =
        0xFFFF; /**< file id used to indicate unresolved reference */
    static constexpr size_t kWordSize =
        sizeof(uint32_t); /**< size of one flash memory word*/

    // API
    static Error::Code openRecord(fds_record_desc_t&  descriptor,
                                  fds_flash_record_t& record);
    static Error::Code closeRecord(fds_record_desc_t& descriptor);

    /**
     * @brief Returns the hashed index to save and load an value from.
     * 
     * @tparam T Data type of the given value
     * @param data Value to hash
     * @return uint16_t CRC16 hash roped to [kRecordKeyMin .. kRecordKeyMax]
     */
    template<class T>
    static constexpr uint16_t getHashedIndex(const T& data)
    {
        uint16_t hash = crc16_compute(reinterpret_cast<const uint8_t*>(&data),
                                      sizeof(T),
                                      nullptr);
        hash %= (Utility::kRecordKeyMax - Utility::kRecordKeyMin);
        hash += Utility::kRecordKeyMin;
        return hash;
    }

    /**
     * @brief Get the Error::Code for the fds error number.
     * 
     * @warning These are not the same error codes as returned
     * by ::Port::Utitilty::getError(). This is due to overlapping
     * error code definitions in the fds implementation.
     * 
     * @param fdsErrorCode Nordic FDS error code
     * @return Error::Code Framework error code
     */
    static constexpr Error::Code getError(uint32_t fdsErrorCode)
    {
        // maps fds codes to Error::Code
        constexpr Error::Code map[] = {
            [FDS_SUCCESS]                = Error::None,
            [FDS_ERR_OPERATION_TIMEOUT]  = Error::Timeout,
            [FDS_ERR_NOT_INITIALIZED]    = Error::NotInitialized,
            [FDS_ERR_UNALIGNED_ADDR]     = Error::Memory,
            [FDS_ERR_INVALID_ARG]        = Error::InvalidParameter,
            [FDS_ERR_NULL_ARG]           = Error::InvalidParameter,
            [FDS_ERR_NO_OPEN_RECORDS]    = Error::Lifetime,
            [FDS_ERR_NO_SPACE_IN_FLASH]  = Error::OutOfResources,
            [FDS_ERR_NO_SPACE_IN_QUEUES] = Error::OutOfResources,
            [FDS_ERR_RECORD_TOO_LARGE]   = Error::TooLarge,
            [FDS_ERR_NOT_FOUND]          = Error::NotFound,
            [FDS_ERR_NO_PAGES]           = Error::OutOfResources,
            [FDS_ERR_USER_LIMIT_REACHED] = Error::OutOfResources,
            [FDS_ERR_CRC_CHECK_FAILED]   = Error::ChecksumFailed,
            [FDS_ERR_BUSY]               = Error::Busy,
            [FDS_ERR_INTERNAL]           = Error::Internal};

        if (fdsErrorCode > sizeof(map)) {
            return Error::Unknown;
        } else {
            return map[fdsErrorCode];
        }
    }
};
}  // namespace IO::Flash
#endif  //__FLASHUTILITY_H__