/**
 * @file AL_FlashFileRecordCollection.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief Container class for iterating over records
 * @version 1.0
 * @date 2020-10-21
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashFile.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct new collection of flash records
 * 
 * @param begin Begin iterator
 * @param end End iterator
 */
IO::Flash::File::RecordCollection::RecordCollection(Iterator&& begin,
                                                    Iterator&& end)
        : beginIt(begin), endIt(end)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Interface for beeing iterable. Can be used with range based loops
 * 
 * @return IO::Flash::File::Iterator& Begin iterator over the RecordCollection
 */
IO::Flash::File::Iterator& IO::Flash::File::RecordCollection::begin()
{
    return beginIt;
}

/**
 * @brief Interface for beeing iterable. Can be used with range based loops
 * 
 * @return IO::Flash::File::Iterator& End iterator over the RecordCollection
 */
IO::Flash::File::Iterator& IO::Flash::File::RecordCollection::end()
{
    return endIt;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------