/**
 * @file AL_FlashCollection.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief generic list stored in flash memory
 * @version 1.0
 * @date 2020-08-11
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_FLASHCOLLECTION_H__
#define __AL_FLASHCOLLECTION_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::Flash
{
template<class T>
class Collection;
}

//--------------------------------- INCLUDES ----------------------------------

#include "FlashUtility.h"
#include "fds.h"

#include <AL_Event.h>
#include <Error.h>
#include <iterator>
#include <AL_FlashFile.h>

namespace IO::Flash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief String identified generic list stored in flash memory.
 * 
 * @warning All writing operations will block until done.
 * This is due to the async underlying "FDS" library not
 * having timeouts.
 * 
 * @details Used to store multiple entries of the same data type
 * in memory and iterate over those flash entries.
 * 
 * @example Easy example to store a list of ints and print it:
 * ```cpp
 * IO::Flash::Collection<int> collection{"myCollection"};
 * CHECK_ERROR(collection.add(1));
 * CHECK_ERROR(collection.add(2));
 * CHECK_ERROR(collection.add(3));
 * for (auto number : collection) {
 *     LOG_I("%d", number);
 * }
 * ```
 * 
 * @param T Type of the elements of the collection
 */
template<class T>
class Collection : public File {
    // delete default constructors
    Collection(const Collection& other) = delete;
    Collection& operator=(const Collection& other) = delete;

private:
    class Iterator;
    friend class Iterator;

public:
    Collection(const char* const name);

    Error::Code add(const T& element);
    Error::Code remove(const T& element);

    Iterator begin();
    Iterator end();
    size_t   size();

private:
    static Error::Code getRecordValue(fds_record_desc_t& descriptor,
                                      T&                 outValue);
};

/**
 * @brief iterates over entries in Flash::Collection.
 * 
 * @details wraps file iterator and filters out descriptor.
 * 
 * @tparam T Type of the elements of the collection
 */
template<class T>
class Collection<T>::Iterator
        : public std::iterator<std::forward_iterator_tag,
                               T,
                               std::ptrdiff_t,
                               std::remove_reference_t<T>> {
public:
    Iterator();
    Iterator(Collection& file);

    bool      operator==(const Iterator& other);
    bool      operator!=(const Iterator& other);
    T         operator*() const;
    Iterator& operator++();

private:
    File::Iterator fileIterator;

    void skipIfFileHeader();
};
}  // namespace IO::Flash

// templates need to include src here
#include "../src/AL_FlashCollection.cpp"
#include "../src/AL_FlashCollectionIterator.cpp"
#endif  //__AL_FLASHCOLLECTION_H__