/**
 * @file FlashUtility.cpp
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

//--------------------------------- INCLUDES ----------------------------------

#include "FlashUtility.h"
#include "PortUtility.h"

#include "fds.h"

#include <AL_Log.h>
#include "AL_FlashFile.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief initialize flash utility.
 * 
 * @warning hard faults on error.
 * 
 */
void IO::Flash::Utility::init()
{
    // File init has to be called before fds_init, it hooks a handler
    // that caches the init_event
    File::init();

    if (fds_init() != NRF_SUCCESS) {
        // escalate, could not init
        CHECK_ERROR(Error::NotInitialized);
    }
}

/**
 * @brief prints flash statistics to info logging.
 * 
 */
void IO::Flash::Utility::printStats()
{
    fds_stat_t stat = {0};
    fds_stat(&stat);

    LOG_I("%d pages available", stat.pages_available);
    LOG_I("%d open records", stat.open_records);
    LOG_I("%d valid records", stat.valid_records);
    LOG_I("%d words reserved", stat.words_reserved);
    LOG_I("%d words used", stat.words_used);
    LOG_I("%d largest contig", stat.largest_contig);
    LOG_I("%d freeable words", stat.freeable_words);
    LOG_I("%d dirty records.", stat.dirty_records);
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Open record with given descriptor
 * 
 * @param descriptor Descriptor of the file to open
 * @param record Out value of the record that was opened
 * @return Error::Code May not find the file
 */
Error::Code IO::Flash::Utility::openRecord(fds_record_desc_t&  descriptor,
                                           fds_flash_record_t& record)
{
    return ::IO::Flash::Utility::getError(
        fds_record_open(&descriptor, &record));
}

/**
 * @brief Close the record
 * 
 * @param descriptor descriptor of the record to close
 * @return Error::Code Might not find the record
 */
Error::Code IO::Flash::Utility::closeRecord(fds_record_desc_t& descriptor)
{
    return ::IO::Flash::Utility::getError(fds_record_close(&descriptor));
}