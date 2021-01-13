/**
 * @file AL_FlashCollectionIterator.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Iterator over flash collections
 * @version 1.0
 * @date 2020-10-21
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashCollection.h"
#include <ScopeExit.h>
#include "AL_Log.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Instantiates an iterator over a flash collection
 * 
 * @details This constructor will create an end iterator
 * 
 * @tparam T Type of the values in the Flash::Collection
 */
template<class T>
IO::Flash::Collection<T>::Iterator::Iterator() : fileIterator {}
{}

/**
 * @brief Instantiates an iterator over a flash collection
 * 
 * @details This constructor will create a start iterator
 * 
 * @tparam T Type of the values in the Flash::Collection
 * @param file File to iterate over
 */
template<class T>
IO::Flash::Collection<T>::Iterator::Iterator(Collection& file)
        : fileIterator {file}
{
    skipIfFileHeader();
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Compares two iterators with each other.
 * 
 * @tparam T Type of the values in the Flash::Collection
 * @param other object to compare to
 * @return true Both iterators are the same
 * @return false The iterators are different
 */
template<class T>
bool IO::Flash::Collection<T>::Iterator::operator==(const Iterator& other)
{
    // just forward to inner iterators
    return fileIterator == other.fileIterator;
}

/**
 * @brief Compares two iterators with each other.
 * 
 * @tparam T Type of the values in the Flash::Collection
 * @param other object to compare to
 * @return true The iterators are different
 * @return false Both iterators are the same
 */
template<class T>
bool IO::Flash::Collection<T>::Iterator::operator!=(const Iterator& other)
{
    // just forward to inner iterators
    return fileIterator != other.fileIterator;
}

/**
 * @brief Dereference iterator. Gets the value the iterator is currently refering to.
 * 
 * @tparam T Type of the values in the Flash::Collection
 * @return T Type of the values in the Flash::Collection
 */
template<class T>
T IO::Flash::Collection<T>::Iterator::operator*() const
{
    auto               descriptor = *fileIterator;
    fds_flash_record_t record     = {0};

    T output {};

    // try to find the record
    if (Utility::openRecord(descriptor, record) != Error::None) {
        LOG_E("Iterator failed to open a file");
        return output;
    }
    auto recordCloser = Patterns::make_scopeExit(
        [&descriptor]() { Utility::closeRecord(descriptor); });

    if ((record.p_header->length_words * Utility::kWordSize) < sizeof(T)) {
        // the data type is too large to fit into that record
        LOG_E("Flash::Collection::Iterator got too small FlashRecord");
        return output;
    }

    // all checks succesful, read value
    // do not attempt to read directly, typed values can only be read uint32 -
    // aligned
    memcpy(&output, static_cast<const char*>(record.p_data), sizeof(T));

    return output;
}

/**
 * @brief Iteration implementation. Will increment the iterator by one entry.
 * 
 * @tparam T Type of the values in the Flash::Collection
 * @return IO::Flash::Collection<T>::Iterator& The iterator
 */
template<class T>
typename IO::Flash::Collection<T>::Iterator&
    IO::Flash::Collection<T>::Iterator::operator++()
{
    ++fileIterator;
    skipIfFileHeader();
    return *this;
}

/**
 * @brief Will skip the current record if it is a file header.
 * 
 * @tparam T Type of the values in the Flash::Collection
 */
template<class T>
void IO::Flash::Collection<T>::Iterator::skipIfFileHeader()
{
    if (fileIterator != IO::Flash::File::Iterator {}) {
        // Check if iterator points to file descriptor.
        // Skip file descriptor.
        auto               descriptor = *fileIterator;
        fds_flash_record_t record     = {0};

        if (Utility::openRecord(descriptor, record) != Error::None) {
            LOG_E("Iterator failed to open a file");
            return;
        }

        auto recordKey = record.p_header->record_key;
        if (recordKey == Utility::kRecordKeyDescriptor) {
            // this is file header, iterate once more
            ++fileIterator;
        }

        if (Utility::closeRecord(descriptor) != Error::None) {
            LOG_E("Iterator failed to close a file");
        }
    }
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------
