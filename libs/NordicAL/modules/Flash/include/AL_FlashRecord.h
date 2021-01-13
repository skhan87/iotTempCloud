/**
 * @file AL_FlashRecord.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief a single record stored in flash
 * @version 1.0
 * @date 2020-03-30
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_FLASHRECORD_H__
#define __AL_FLASHRECORD_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashFile.h"
#include "Error.h"
#include "Observable.h"
#include "fds.h"

#include <AL_Event.h>
#include <cstddef>

namespace IO::Flash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief Record in flash memory
 * 
 * @warning All writing operations will block until done.
 * This is due to the async underlying "FDS" library not
 * having timeouts.
 * 
 * @details Single string identified entry in flash.
 * You can instantiate it with any type and thereby write
 * and read them from and to flash.
 * If you want to be notified when another class changes
 * a record, just hook a Observer to it.
 *
 * @tparam T data type of the record
 */
template<class T>
class Record : Patterns::Observable<T> {
    // delete default constructors
    Record()                    = delete;
    Record(const Record& other) = delete;
    Record& operator=(const Record& other) = delete;

public:
    constexpr Record(const char* const identifier, File& file);

    Error::Code tryGet(T& value);
    Error::Code trySet(const T& value);

private:
    const char* const name; /**< name of the string identified record */
    File&             file; /**< file to store this record in */

    constexpr uint16_t getRecordIndex();
    constexpr size_t   lengthWords();
    constexpr size_t   lengthBytes();
    Error::Code        tryOpen(fds_record_desc_t&  descriptor,
                               fds_flash_record_t& record);
};
}  // namespace IO::Flash

#include "../src/AL_FlashRecord.cpp"
#endif  //__AL_FLASHRECORD_H__