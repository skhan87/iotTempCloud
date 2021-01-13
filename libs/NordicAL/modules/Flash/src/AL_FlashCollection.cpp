/**
 * @file AL_FlashCollection.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief generic list stored in flash
 * @version 1.0
 * @date 2020-08-11
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashCollection.h"

#include <FlashUtility.h>
#include <ScopeExit.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Make a new Flash::Collection instance.
 * 
 * @details This works lazy loading, the variable is only written
 * or read for the first time when it is used.
 * 
 * @tparam T Type of the elements of the collection
 * @param name String identifier of the collection
 */
template<class T>
IO::Flash::Collection<T>::Collection(const char* const name) : File(name)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Add new element to the collection
 * 
 * @warning Will block until done. Refer to class doc.
 * 
 * @tparam T Type of the elements of the collection
 * @param element Value of the element to add
 * @return Error::Code Might fail while interacting with underlaying libs.
 */
template<class T>
Error::Code IO::Flash::Collection<T>::add(const T& element)
{
    std::unique_ptr<uint8_t[]> data {new uint8_t[sizeof(T)]};
    memcpy(data.get(), reinterpret_cast<const uint8_t*>(&element), sizeof(T));
    return createRecord(Utility::getHashedIndex<T>(element),
                        std::move(data),
                        sizeof(T));
}

/**
 * @brief Tries to delete element with given value.
 * 
 * @warning Will block until done. Refer to class doc.
 * 
 * @tparam T Type of the elements of the collection
 * @param element Value of the element to delete.
 * @return Error::Code Might not find the given record.
 */
template<class T>
Error::Code IO::Flash::Collection<T>::remove(const T& element)
{
    // try to find the entry by its hashed value
    for (auto recordDescriptor :
         findByRecordKey(Utility::getHashedIndex(element))) {
        T otherValue {};
        RETURN_ON_ERROR(getRecordValue(recordDescriptor, otherValue));
        if (otherValue == element) {
            return removeRecord(recordDescriptor);
        }
    }

    return Error::NotFound;
}

/**
 * @brief Begin iterator so the range based operator can be used with this.
 * 
 * @tparam T Type of the elements of the collection
 * @return IO::Flash::Collection<T>::Iterator Begin iterator for the collection.
 */
template<class T>
typename IO::Flash::Collection<T>::Iterator IO::Flash::Collection<T>::begin()
{
    return IO::Flash::Collection<T>::Iterator {*this};
}

/**
 * @brief End iterator for range based loops.
 * 
 * @tparam T Type of the elements of the collection
 * @return IO::Flash::Collection<T>::Iterator End Iterator for the file.
 */
template<class T>
typename IO::Flash::Collection<T>::Iterator IO::Flash::Collection<T>::end()
{
    return IO::Flash::Collection<T>::Iterator {};
}

/**
 * @brief Iterates all existing entries and returns their count.
 * 
 * @tparam T Type of the elements of the collection
 * @return size_t Number of elements in the collection
 */
template<class T>
size_t IO::Flash::Collection<T>::size()
{
    size_t cnt = 0;
    for (auto element : *this) {
        static_cast<void>(element);  // surpress unused warning
        ++cnt;
    }
    return cnt;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Reads the value from the given descriptor.
 * 
 * @tparam T Type of the FlashCollection elements
 * @param descriptor Valid descriptor for flash record
 * @param outValue Variable to hold the value read from flash
 * @return Error::Code Might not find the record.
 */
template<class T>
Error::Code
    IO::Flash::Collection<T>::getRecordValue(fds_record_desc_t& descriptor,
                                             T&                 outValue)
{
    fds_flash_record_t record = {0};

    RETURN_ON_ERROR(Utility::openRecord(descriptor, record));
    // closes record when scope is left
    auto recordCloser = Patterns::make_scopeExit(
        [&descriptor]() { Utility::closeRecord(descriptor); });

    auto ref = reinterpret_cast<uint8_t*>(&outValue);

    if ((record.p_header->length_words * Utility::kWordSize) < sizeof(T)) {
        // the data type is too large to fit into that record
        return Error::TooLarge;
    }

    memcpy(ref, record.p_data, sizeof(T));

    recordCloser.deactivate();
    return Utility::closeRecord(descriptor);
}
