/**
 * @file AL_FlashFileIterator.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief iterator for records in a file
 * @version 1.0
 * @date 2020-08-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashFile.h"
#include <FlashUtility.h>
#include <AL_Log.h>
#include "ScopeExit.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct an end iterator
 * 
 */
IO::Flash::File::Iterator::Iterator()
        : iterationMode(Mode::End), fileToSearch(nullptr),
          recordKey(Utility::kRecordKeyReserved), currentSearch({0}),
          lastDescriptor({0})
{}

/**
 * @brief Construct an iterator over all records in a file
 * 
 * @param file File to iterate over
 */
IO::Flash::File::Iterator::Iterator(File& file)
        : iterationMode(Mode::WholeFile), fileToSearch(&file),
          recordKey(Utility::kRecordKeyReserved), currentSearch({0}),
          lastDescriptor({0})
{
    // dereference first entry
    nextWholeFile();
}

/**
 * @brief Construct an iterator over the records with given recordKey in given file
 * 
 * @param file File to iterate over
 * @param recordKey Only show records with given key
 */
IO::Flash::File::Iterator::Iterator(File& file, uint16_t recordKey)
        : iterationMode(Mode::ByRecordKey), fileToSearch(&file),
          recordKey(recordKey), currentSearch({0}), lastDescriptor({0})
{
    // dereference first entry
    nextByRecordKey();
}

/**
 * @brief Construct an interator over all records (regardless of file) with given recordKey.
 * 
 * @param recordKey RecordKEy (not RecordId) to search for.
 */
IO::Flash::File::Iterator::Iterator(uint16_t recordKey)
        : iterationMode(Mode::AllFiles), fileToSearch(nullptr),
          recordKey(recordKey), currentSearch({0}), lastDescriptor({0})
{
    // dereference first entry
    nextAllFiles();
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

/**
 * @brief Implement the equals operator for end condition of for based loop.
 * 
 * @param other Object to compare to
 * @return true Both iterators are the same
 * @return false The iterators are different
 */
bool IO::Flash::File::Iterator::operator==(const Iterator& other)
{
    return ((iterationMode == other.iterationMode) &&
            (fileToSearch == other.fileToSearch) &&
            (currentSearch.p_addr == other.currentSearch.p_addr) &&
            (currentSearch.page == other.currentSearch.page)) ||
           (other.iterationMode == Mode::End && iterationMode == Mode::End);
}

/**
 * @brief Implement the unequals operator for end condition of for based loop.
 * 
 * @param other Object to compare to
 * @return true The iterators are different
 * @return false Both iterators are the same
 */
bool IO::Flash::File::Iterator::operator!=(const Iterator& other)
{
    return !(*this == other);
}

/**
 * @brief Dereferences the iterator to get the record descriptor
 * 
 * @return fds_record_desc_t Record descriptor of the current iterator
 */
fds_record_desc_t IO::Flash::File::Iterator::operator*() const
{
    return lastDescriptor;
}

/**
 * @brief Iterate operator. Will search for the next record.
 * 
 * @return Iterator& Itself
 */
IO::Flash::File::Iterator& IO::Flash::File::Iterator::operator++()
{
    switch (iterationMode) {
        case Mode::End: {
            // stay the same, end iterator
            break;
        }
        case Mode::WholeFile: {
            nextWholeFile();
            break;
        }
        case Mode::ByRecordKey: {
            nextByRecordKey();
            break;
        }
        case Mode::AllFiles: {
            nextAllFiles();
            break;
        }
        default: {
            // should never get here
            LOG_E("Error while iterating files");
            break;
        }
    }

    return *this;
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Implements incrementation when whole file is searched
 * 
 */
void IO::Flash::File::Iterator::nextWholeFile()
{
    uint16_t fileId;
    if (fileToSearch->getId(fileId) != Error::None) {
        // invalidate this iterator
        iterationMode = Mode::End;
        fileToSearch  = nullptr;
        return;
    }
    auto result =
        fds_record_find_in_file(fileId, &lastDescriptor, &currentSearch);
    switch (result) {
        case NRF_SUCCESS: {
            // found record, keep everything as is.
            // last descriptor must now contaiin a valid descriptor
            break;
        }
        case FDS_ERR_NOT_FOUND:
        case FDS_ERR_NOT_INITIALIZED:
        case FDS_ERR_NULL_ARG:
        default: {
            // invalidate
            iterationMode  = Mode::End;
            fileToSearch   = nullptr;
            lastDescriptor = {0};
            break;
        }
    }
}

/**
 * @brief Increments when file is searched with record key
 * 
 */
void IO::Flash::File::Iterator::nextByRecordKey()
{
    uint16_t fileId;
    if (fileToSearch->getId(fileId) != Error::None) {
        // invalidate this iterator
        iterationMode = Mode::End;
        fileToSearch  = nullptr;
        return;
    }
    auto result =
        fds_record_find(fileId, recordKey, &lastDescriptor, &currentSearch);
    switch (result) {
        case NRF_SUCCESS: {
            // found record, keep everything as is.
            // last descriptor must now contaiin a valid descriptor
            break;
        }
        case FDS_ERR_NOT_FOUND:
        case FDS_ERR_NOT_INITIALIZED:
        case FDS_ERR_NULL_ARG:
        default: {
            // invalidate
            iterationMode  = Mode::End;
            fileToSearch   = nullptr;
            lastDescriptor = {0};
            break;
        }
    }
}

/**
 * @brief Increments when all files are searched for a record id
 * 
 */
void IO::Flash::File::Iterator::nextAllFiles()
{
    auto result =
        fds_record_find_by_key(recordKey, &lastDescriptor, &currentSearch);
    switch (result) {
        case NRF_SUCCESS: {
            // found record, keep everything as is.
            // last descriptor must now contaiin a valid descriptor
            break;
        }
        case FDS_ERR_NOT_FOUND:
        case FDS_ERR_NOT_INITIALIZED:
        case FDS_ERR_NULL_ARG:
        default: {
            // invalidate
            iterationMode = Mode::End;
            fileToSearch  = nullptr;
            recordKey     = Utility::kRecordKeyReserved;
            break;
        }
    }
}

//---------------------------- STATIC FUNCTIONS -------------------------------