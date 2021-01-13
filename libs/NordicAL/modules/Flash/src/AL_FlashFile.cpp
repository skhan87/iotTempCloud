/**
 * @file AL_FlashFile.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief file abstraction for fds library
 * @version 1.0
 * @date 2020-08-14
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include "AL_FlashFile.h"

#include "FlashUtility.h"
#include "PortUtility.h"
#include "FunctionScopeTimer.h"

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

/**
 * @brief Construct a new File.
 * 
 * @warning Does not write anything into flash until entries foor the file are created.
 * 
 * @param name String identifier for the file.
 */
IO::Flash::File::File(const char* const name)
        : name(name), lastKnownFileId(Utility::kFileIdInvalid)
{}

/**
 * @brief Default constructor for heap chunks
 * 
 */
IO::Flash::File::HeapChunk::HeapChunk()
        : result(Error::Unknown), onFinish(getFinishEvents()),
          isFree(getFreeEvents()), operation(AsyncOperation::None), recordId(0),
          fileId(Utility::kFileIdInvalid),
          recordKey(Utility::kRecordKeyReserved)
{
    // make this heap chunk available
    isFree.trigger();
}

//--------------------------- EXPOSED FUNCTIONS -------------------------------

/**
 * @brief Gets the name of the file
 * 
 * @return const char* const 
 */
const char* const IO::Flash::File::getName()
{
    return name;
}

/**
 * @brief Deletes all entries in this files
 * 
 * @return Error::Code Might not find the file.
 */
Error::Code IO::Flash::File::clear()
{
    uint16_t fileId;
    auto     errCode = getId(fileId);
    // if file is not found it is cleared, otherwise return error
    if (errCode == Error::NotFound) {
        return Error::None;
    } else if (errCode != Error::None) {
        return errCode;
    }

    HeapChunk* chunk {nullptr};
    RETURN_ON_ERROR(allocRecordSpace(chunk, AsyncOperation::DeleteFile));

    // call clean on the heapchunk when this function returns
    auto stackGuard = Patterns::make_scopeExit([chunk]() { chunk->free(); });
    chunk->fileId   = fileId;
    RETURN_ON_ERROR(::IO::Flash::Utility::getError(fds_file_delete(fileId)));
    RETURN_ON_ERROR(chunk->onFinish.await(RTOS::Infinity));

    if (chunk->result != Error::None) {
        return chunk->result;
    }

    return Error::None;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------

/**
 * @brief Try to create a record.
 * 
 * @warning Will block until done.
 * Timeout can not be implemented due to underlying library.
 *
 * @param recordKey Record key to create the new record with
 * @param buffer Buffered array to write
 * @param lenBytes Length of the buffer in bytes
 * 
 * @return Error::Code Might fail because of invalid parameters timeout.
 */
Error::Code
    IO::Flash::File::createRecord(uint16_t                         recordKey,
                                  std::unique_ptr<const uint8_t[]> buffer,
                                  size_t                           lenBytes)
{
    fds_record_desc_t descriptor = {0};

    auto lenWords = (lenBytes + Utility::kWordSize - 1) / Utility::kWordSize;

    uint16_t fileId;
    auto     result = getId(fileId);
    if (result == Error::NotFound) {
        // file not yet existing, create
        RETURN_ON_ERROR(create());
        RETURN_ON_ERROR(getId(fileId));
    } else if (result != Error::None) {
        // different error
        return result;
    }

    // get dynamicly allocated buffer
    HeapChunk* chunk {nullptr};
    RETURN_ON_ERROR(allocRecordSpace(chunk, AsyncOperation::Write));

    // call clean on the heapchunk when this function returns
    auto stackGuard  = Patterns::make_scopeExit([chunk]() { chunk->free(); });
    chunk->heapPtr   = std::move(buffer);
    chunk->fileId    = fileId;
    chunk->recordKey = recordKey;

    fds_record_t tmpRecord = {.file_id = fileId,
                              .key     = recordKey,
                              .data    = {.p_data       = chunk->heapPtr.get(),
                                       .length_words = lenWords}};

    chunk->onFinish.reset();
    auto nordicResult = fds_record_write(&descriptor, &tmpRecord);
    if (nordicResult != NRF_SUCCESS) {
        if (nordicResult == FDS_ERR_NO_SPACE_IN_FLASH) {
            // flash is full, try to call gc
            bool recordsFreed = false;
            RETURN_ON_ERROR(callGarbageCollection(recordsFreed));
            if (recordsFreed) {
                // if it fails now, there is nothing left to do, flash might be full
                RETURN_ON_ERROR(::IO::Flash::Utility::getError(
                    fds_record_write(&descriptor, &tmpRecord)));
            } else {
                // no records to free, flash is just full
                return Error::OutOfResources;
            }
        } else {
            return ::IO::Flash::Utility::getError(nordicResult);
        }
    }
    // wait for write to finish
    RETURN_ON_ERROR(chunk->onFinish.await(RTOS::Infinity));

    result = chunk->result;
    return result;
}

/**
 * @brief Tries to update an existing record.
 * 
 * @warning Will block until done.
 * Timeout can not be implemented due to underlying library.
 * 
 * @param descriptor Descriptor to existing record that shall be updated
 * @param newRecordKey New recordKey to use
 * @param buffer Buffer with the new data to use
 * @param lenBytes Length of the buffer
 * @return Error::Code Might time out or invalid parameters
 */
Error::Code
    IO::Flash::File::updateRecord(fds_record_desc_t&               descriptor,
                                  uint16_t                         newRecordKey,
                                  std::unique_ptr<const uint8_t[]> buffer,
                                  size_t                           lenBytes)
{
    auto lenWords = (lenBytes + Utility::kWordSize - 1) / Utility::kWordSize;

    uint16_t fileId;
    auto     result = getId(fileId);
    if (result == Error::NotFound) {
        // file not yet existing, create
        RETURN_ON_ERROR(create());
        RETURN_ON_ERROR(getId(fileId));
    } else if (result != Error::None) {
        return result;
    }

    // get dynamicly allocated buffer
    HeapChunk* chunk {nullptr};
    RETURN_ON_ERROR(allocRecordSpace(chunk, AsyncOperation::Update));
    // call clean when this function returns
    auto stackGuard = Patterns::make_scopeExit([chunk]() { chunk->free(); });
    uint32_t oldRecordId;
    RETURN_ON_ERROR(::IO::Flash::Utility::getError(
        fds_record_id_from_desc(&descriptor, &oldRecordId)));
    chunk->recordId  = oldRecordId;
    chunk->fileId    = fileId;
    chunk->recordKey = newRecordKey;
    chunk->heapPtr   = std::move(buffer);

    fds_record_t tmpRecord = {.file_id = fileId,
                              .key     = newRecordKey,
                              .data    = {.p_data       = chunk->heapPtr.get(),
                                       .length_words = lenWords}};

    chunk->onFinish.reset();
    auto nordicResult = fds_record_update(&descriptor, &tmpRecord);
    if (nordicResult != NRF_SUCCESS) {
        if (nordicResult == FDS_ERR_NO_SPACE_IN_FLASH) {
            // flash is full, try to call gc
            bool recordsFreed = false;
            RETURN_ON_ERROR(callGarbageCollection(recordsFreed));
            if (recordsFreed) {
                // if it fails now, there is nothing left to do, flash might be full
                RETURN_ON_ERROR(::IO::Flash::Utility::getError(
                    fds_record_update(&descriptor, &tmpRecord)));
            } else {
                // no records to free, flash is just full
                return Error::OutOfResources;
            }
        } else {
            // some other plain old error
            return ::IO::Flash::Utility::getError(nordicResult);
        }
    }
    RETURN_ON_ERROR(chunk->onFinish.await(RTOS::Infinity));

    result = chunk->result;
    return result;
}

/**
 * @brief Removes the record with the given descriptor.
 * 
 * @warning Will block until done.
 * Timeout can not be implemented due to underlying library.
 * 
 * @param descriptor Descriptor to existing FileRecord
 * @return Error::Code Might timeout or invalid parameters
 */
Error::Code IO::Flash::File::removeRecord(fds_record_desc_t& descriptor)
{
    uint16_t fileId;
    // Return Not found if file does noot even exist
    RETURN_ON_ERROR(getId(fileId));

    // get dynamicly allocated buffer
    HeapChunk* chunk {nullptr};
    RETURN_ON_ERROR(allocRecordSpace(chunk, AsyncOperation::DeleteRecord));

    // call clean when this function returns
    auto stackGuard = Patterns::make_scopeExit([chunk]() { chunk->free(); });

    uint32_t oldRecordId;
    RETURN_ON_ERROR(::IO::Flash::Utility::getError(
        fds_record_id_from_desc(&descriptor, &oldRecordId)));
    chunk->recordId = oldRecordId;
    chunk->fileId   = fileId;

    chunk->onFinish.reset();
    auto nordicCode = fds_record_delete(&descriptor);
    if (nordicCode != NRF_SUCCESS) {
        LOG_E("fds_record_delete failed: %u", nordicCode);
    }
    RETURN_ON_ERROR(::IO::Flash::Utility::getError(nordicCode));
    RETURN_ON_ERROR(chunk->onFinish.await(RTOS::Infinity));

    if (chunk->result != Error::None) {
        return chunk->result;
    }

    auto result = chunk->result;
    return result;
}

/**
 * @brief Get an collection over all records in this file with given recordKey.
 * 
 * @param recordKey RecordKey to search for (not recordId)
 * @return RecordCollection Will return a collection with all Records with the given RecordKey
 */
IO::Flash::File::RecordCollection
    IO::Flash::File::findByRecordKey(uint16_t recordKey)
{
    return RecordCollection {Iterator {*this, recordKey}, Iterator {}};
}

/**
 * @brief Gets the FDS file_id for this file
 * 
 * @param id Output for the id if found
 * @return Error::Code might not find the given file
 */
Error::Code IO::Flash::File::getId(uint16_t& id)
{
    if (lastKnownFileId != Utility::kFileIdInvalid) {
        // file was already found and its id can just be used
        id = lastKnownFileId;
        return Error::None;
    } else {
        // search the file in flash
        for (auto iter = Iterator(Utility::kRecordKeyDescriptor);
             iter != Iterator();
             ++iter) {
            auto               descriptor = *iter;
            fds_flash_record_t record     = {0};

            RETURN_ON_ERROR(Utility::openRecord(descriptor, record));

            // try to find the descriptor
            uint16_t fileId = Utility::kFileIdInvalid;
            if (strcmp(name, static_cast<const char*>(record.p_data)) == 0) {
                // found the right one
                fileId = record.p_header->file_id;
            }

            RETURN_ON_ERROR(Utility::closeRecord(descriptor));

            if (fileId != Utility::kFileIdInvalid) {
                lastKnownFileId = fileId;
                id              = fileId;
                return Error::None;
            }
        }

        id = Utility::kFileIdInvalid;
        return Error::NotFound;
    }
}

/**
 * @brief Create this file in flash
 * 
 * @warning Will block until done.
 * Timeout can not be implemented due to underlying library.
 * 
 * @return Error::Code All files might be in use
 */
Error::Code IO::Flash::File::create()
{
    // not found, create new file desriptor and file
    for (auto fileId = Utility::kFileIdMin; fileId <= Utility::kFileIdMax;
         ++fileId) {
        // check whether there is at least one record
        lastKnownFileId = fileId;
        if (Iterator(*this) == Iterator()) {
            // file iterator does not yield any results, fileId is unused
            size_t                     dataLength = strlen(name) + 1;
            std::unique_ptr<uint8_t[]> dataPtr {new uint8_t[dataLength]};
            memcpy(dataPtr.get(), name, dataLength);
            RETURN_ON_ERROR(createRecord(Utility::kRecordKeyDescriptor,
                                         std::move(dataPtr),
                                         dataLength));
            return Error::None;
        }
    }

    // all files used, very unlikely to get here without a bug
    return Error::OutOfResources;
}

/**
 * @brief Cleans up the chunk and resets to defaults.
 * 
 */
void IO::Flash::File::HeapChunk::free()
{
    result = Error::Unknown;
    onFinish.reset();
    operation = AsyncOperation::None;
    recordId  = 0;
    fileId    = Utility::kFileIdInvalid;
    recordKey = Utility::kRecordKeyReserved;
    heapPtr.reset();

    // only now free the chunk
    isFree.trigger();
}

//---------------------------- STATIC FUNCTIONS -------------------------------

/**
 * @brief Get the Chunks holding the memory for flash operations.
 * 
 * @return std::array<HeapChunk, FDS_OP_QUEUE_SIZE>& 
 */
std::array<IO::Flash::File::HeapChunk, FDS_OP_QUEUE_SIZE>&
    IO::Flash::File::getChunks()
{
    static std::array<HeapChunk, FDS_OP_QUEUE_SIZE> chunks;
    return chunks;
}

/**
 * @brief Get Events that mark finishing of an operation in a heapchunk.
 * 
 * @details Lazy loading for proper order of initialization.
 * 
 * @return RTOS::EventGroup& Event group marks whether heap chunks are done.
 */
RTOS::EventGroup& IO::Flash::File::getFinishEvents()
{
    static RTOS::EventGroup group {};
    return group;
}

/**
 * @brief Get Events that guard the HeapChunks.
 * 
 * @details Lazy loading for proper order of initialization.
 * 
 * @return RTOS::EventGroup& Event group that guards the HeapChunks
 */
RTOS::EventGroup& IO::Flash::File::getFreeEvents()
{
    static RTOS::EventGroup group {};
    return group;
}

/**
 * @brief Registers flash handler
 * 
 */
void IO::Flash::File::init()
{
    if (fds_register(&handler) != NRF_SUCCESS) {
        // escalate, could not init
        CHECK_ERROR(Error::NotInitialized);
    }
}

/**
 * @brief handler for flash events
 * 
 * @param evt Nordic SDK event. Contains event information
 * 
 * @details Forwards to subsequent handlers
 */
void IO::Flash::File::handler(fds_evt_t const* evt)
{
    switch (evt->id) {
        case FDS_EVT_INIT: {
            if (evt->result == FDS_SUCCESS) {
                LOG_I("Flash module initialized");
            } else {
                // escalate errors on initialization
                // can not synchronize to init function, since it
                // is called before scheduler is started.
                // This is not an FDS error code, but a common SD error code.
                CHECK_ERROR(::Port::Utility::getError(evt->result));
            }
            return;
        }
        case FDS_EVT_WRITE:
        case FDS_EVT_UPDATE: {
            handlerWriteUpdate(evt);
            break;
        }
        case FDS_EVT_DEL_RECORD: {
            handlerDeleteRecord(evt);
            break;
        }
        case FDS_EVT_DEL_FILE: {
            handlerDeleteFile(evt);
            break;
        }
        case FDS_EVT_GC: {
            handlerGarbageCollection(evt);
            break;
        }
        default:
            LOG_E("unhandled FDS event: %u", evt->id);
            return;
    }
}

/**
 * @brief Handles write and update events.
 * 
 * @details Will search for the chunk used for this operation.
 * Then copies the event data into chunk and triggers its
 * onFinish event.
 * 
 * @param evt Nordic SDK event. Contains event information
 */
void IO::Flash::File::handlerWriteUpdate(fds_evt_t const* evt)
{
    for (auto& chunk : getChunks()) {
        if (!chunk.isFree.wasTriggered() &&
            (chunk.operation == AsyncOperation::Update ||
             chunk.operation == AsyncOperation::Write) &&
            chunk.fileId == evt->write.file_id &&
            chunk.recordKey == evt->write.record_key) {
            // update recordId so the blocking function can read it
            chunk.recordId = evt->write.record_id;
            chunk.result   = ::Port::Utility::getError(evt->result);
            chunk.onFinish.trigger();
            return;
        }
    }
    LOG_W("timeouted write returned with error code: %u", evt->result);
}

/**
 * @brief Handles delete record callbacks.
 * 
 * @details Will search for the chunk used for this operation.
 * Then copies the event data into chunk and triggers its
 * onFinish event.
 * 
 * @param evt Nordic SDK event. Contains event information
 */
void IO::Flash::File::handlerDeleteRecord(fds_evt_t const* evt)
{
    for (auto& chunk : getChunks()) {
        if (!chunk.isFree.wasTriggered() &&
            chunk.operation == AsyncOperation::DeleteRecord &&
            chunk.recordId == evt->del.record_id) {
            // update recordId so the blocking function can read it
            chunk.result = ::Port::Utility::getError(evt->result);
            chunk.onFinish.trigger();
            return;
        }
    }
    LOG_W("timeouted deleteRecord returned with error code: %u", evt->result);
}

/**
 * @brief Handles delete file callbacks.
 * 
 * @details Will search for the chunk used for this operation.
 * Then copies the event data into chunk and triggers its
 * onFinish event.
 * 
 * @param evt Nordic SDK event. Contains event information
 */
void IO::Flash::File::handlerDeleteFile(fds_evt_t const* evt)
{
    for (auto& chunk : getChunks()) {
        if (!chunk.isFree.wasTriggered() &&
            chunk.operation == AsyncOperation::DeleteFile &&
            chunk.fileId == evt->del.file_id) {
            // update recordId so the blocking function can read it
            chunk.result = ::Port::Utility::getError(evt->result);
            chunk.onFinish.trigger();
            return;
        }
    }
    LOG_W("timeouted deleteFile returned with error code: %u", evt->result);
}

/**
 * @brief Handles garbage collection callbacks.
 * 
 * @details Will search for the chunk used for this operation.
 * Then copies the event data into chunk and triggers its
 * onFinish event.
 * 
 * @param evt Nordic SDK event. Contains event information
 */
void IO::Flash::File::handlerGarbageCollection(fds_evt_t const* evt)
{
    for (auto& iter : getChunks()) {
        if (!iter.isFree.wasTriggered() &&
            iter.operation == AsyncOperation::GarbageCollection) {
            // there must not be more than one garbage collection in the queue
            iter.result = ::Port::Utility::getError(evt->result);
            iter.onFinish.trigger();
            return;
        }
    }
    LOG_W("timeouted gc returned with error code: %u", evt->result);
}

/**
 * @brief calls the garbage collector if needed
 * 
 * @warning Will block until done.
 * Timeout can not be implemented due to underlying library.
 * 
 * @param ran True when it actually ran, false if there is nothing to free
 * 
 * @details Triggers the garabge collection only, if there are records to free.
 * 
 * @return Error::Code Might Timeout
 */
Error::Code IO::Flash::File::callGarbageCollection(bool& ran)
{
    fds_stat_t stat = {0};
    fds_stat(&stat);

    if (stat.freeable_words > 0) {
        HeapChunk* chunk {nullptr};
        RETURN_ON_ERROR(
            allocRecordSpace(chunk, AsyncOperation::GarbageCollection));
        auto stackGuard =
            Patterns::make_scopeExit([chunk]() { chunk->free(); });

        RETURN_ON_ERROR(::IO::Flash::Utility::getError(fds_gc()));
        RETURN_ON_ERROR(chunk->onFinish.await(RTOS::Infinity));
        auto result = chunk->result;
        ran         = true;
        return result;
    } else {
        // no garbage collection needed or possible
        ran = false;
        return Error::None;
    }
}

/**
 * @brief Tries to get record space for putting stuff in the operation queue.
 * 
 * @param chunk Index of chunk which was allocated for usage.
 * @param operation Which operation to allocate the HeapChunk for.
 * @return Error::Code If wait for free chunks fails.
 */
Error::Code
    IO::Flash::File::allocRecordSpace(IO::Flash::File::HeapChunk*&    chunk,
                                      IO::Flash::File::AsyncOperation operation)
{
    while (1) {
        // wait for one entry to be freed
        RETURN_ON_ERROR(getFreeEvents().await(getFreeEventList(),
                                              RTOS::Infinity,
                                              RTOS::EventGroup::WaitMode::Or));
        for (auto& iterChunk : getChunks()) {
            if (iterChunk.isFree.wasTriggered()) {
                iterChunk.isFree.reset();
                iterChunk.operation = operation;
                chunk               = &iterChunk;
                return Error::None;
            }
        }
        // if function gets here, someone else also waited for the event and took the chunk
    }
}

/**
 * @brief Get list of all isFree events in chunks.
 * 
 * @details Can be used to block until a chunk is available.
 * 
 * @return RTOS::EventList<FDS_OP_QUEUE_SIZE> 
 */
RTOS::EventList<FDS_OP_QUEUE_SIZE> IO::Flash::File::getFreeEventList()
{
    RTOS::EventList<FDS_OP_QUEUE_SIZE> list {};
    size_t                             i = 0;
    for (auto& chunk : getChunks()) {
        list[i++] = &chunk.isFree;
    }

    return list;
}