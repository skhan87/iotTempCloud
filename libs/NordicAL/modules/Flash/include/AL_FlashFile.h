/**
 * @file AL_FlashFile.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief abstraction for files stored in flash
 * @version 1.0
 * @date 2020-08-12
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#ifndef __AL_FLASHFILE_H__
#define __AL_FLASHFILE_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace IO::Flash
{
class File;
}

//--------------------------------- INCLUDES ----------------------------------

#include "fds.h"
#include "sdk_config.h"

#include <AL_Event.h>
#include <Error.h>
#include <array>
#include <iterator>
#include <memory>

namespace IO::Flash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief File stored in flash.
 * 
 * @warning All writing operations will block until done.
 * This is due to the async underlying "FDS" library not
 * having timeouts.
 * 
 * @details This class enables the user to create files in flash and
 * work with them.
 * FlashCollection is a file and FlashRecord stores its entries in a file.
 * It is identified in flash by its string name.
 * 
 */
class File {
    // delete default constructors
    File()                  = delete;
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;

    class Iterator;
    class RecordCollection;

    friend class Iterator;
    friend class Utility;
    template<class T>
    friend class Record;

public:
    File(const char* const name);
    const char* const getName();
    Error::Code       clear();

protected:
    Error::Code createRecord(uint16_t                         recordKey,
                             std::unique_ptr<const uint8_t[]> buffer,
                             const size_t                     lenBytes);
    Error::Code updateRecord(fds_record_desc_t&               descriptor,
                             uint16_t                         newRecordKey,
                             std::unique_ptr<const uint8_t[]> buffer,
                             const size_t                     lenBytes);

    Error::Code removeRecord(fds_record_desc_t& descriptor);

    RecordCollection findByRecordKey(uint16_t recordKey);

private:
    Error::Code getId(uint16_t& id);
    Error::Code create();

    static void init();

    const char* const name; /**< filename */
    uint16_t
        lastKnownFileId; /**< once file is found by name, file id is stored in here */

    /**
     * @brief Possible async operations to perform.
     * 
     */
    enum class AsyncOperation {
        Init              = FDS_EVT_INIT,
        Write             = FDS_EVT_WRITE,
        Update            = FDS_EVT_UPDATE,
        DeleteRecord      = FDS_EVT_DEL_RECORD,
        DeleteFile        = FDS_EVT_DEL_FILE,
        GarbageCollection = FDS_EVT_GC,
        None
    };

    /**
     * @brief one heap chunk that got allocated for writing a record.
     * heapPtr is freed, when associated recordId write operation returned an
     * event.
     */
    struct HeapChunk {
        Error::Code    result; /**< result of the operation */
        RTOS::Event    onFinish; /**< event that gets triggerd on completion */
        RTOS::Event    isFree; /**< whether this chunk is used */
        AsyncOperation operation; /**< which async operation is bufferd here */
        uint32_t
                 recordId; /**< record id of the record. On update this gets updated from the handler */
        uint16_t fileId; /**< file id if needed */
        uint16_t recordKey; /**< record key if needed */
        std::unique_ptr<const uint8_t[]>
            heapPtr; /**< pointer to allocated heap where raw data is stored */

        HeapChunk();

        void free();
    };

    static std::array<HeapChunk, FDS_OP_QUEUE_SIZE>& getChunks();
    static RTOS::EventGroup&                         getFinishEvents();
    static RTOS::EventGroup&                         getFreeEvents();
    static void handler(fds_evt_t const* evt);
    static void handlerWriteUpdate(fds_evt_t const* evt);
    static void handlerDeleteRecord(fds_evt_t const* evt);
    static void handlerDeleteFile(fds_evt_t const* evt);
    static void handlerGarbageCollection(fds_evt_t const* evt);

    static Error::Code callGarbageCollection(bool& ran);
    static Error::Code allocRecordSpace(HeapChunk*&    iChunk,
                                        AsyncOperation operation);
    static RTOS::EventList<FDS_OP_QUEUE_SIZE> getFreeEventList();
};

/**
 * @brief iterates over entires in flash files
 * 
 */
class File::Iterator : public std::iterator<std::forward_iterator_tag,
                                            fds_record_desc_t,
                                            std::ptrdiff_t,
                                            fds_record_desc_t> {
public:
    Iterator();
    Iterator(File& file);
    Iterator(File& file, uint16_t recordKey);
    Iterator(uint16_t recordId);

    bool              operator==(const Iterator& other);
    bool              operator!=(const Iterator& other);
    fds_record_desc_t operator*() const;
    Iterator&         operator++();

private:
    enum class Mode {
        End, /**< used when end of iteration */
        WholeFile, /**< iterates the whole file */
        ByRecordKey, /**< only iterates the entries in the file with given record keys */
        AllFiles /**< searches by record id over all files */
    };

    Mode     iterationMode; /**< determines how records are iterated */
    File*    fileToSearch; /**< file which is searched by this iterator, or nullptr when end element */
    uint16_t recordKey; /**< id of the record searched for */
    fds_find_token_t
                      currentSearch; /**< used with nordic fds functions for iterating searches */
    fds_record_desc_t lastDescriptor; /**< descriptor of the last search result */

    void nextWholeFile();
    void nextByRecordKey();
    void nextAllFiles();
};

/**
 * @brief contains begin and end iterator for a search.
 * 
 */
class File::RecordCollection {
    // delete default constructors
    RecordCollection() = delete;

public:
    RecordCollection(const RecordCollection& other) = default;
    RecordCollection& operator=(const RecordCollection& other) = default;

    RecordCollection(Iterator&& begin, Iterator&& end);

    Iterator& begin();
    Iterator& end();

private:
    Iterator beginIt;
    Iterator endIt;
};
}  // namespace IO::Flash
#endif  //__AL_FLASHFILE_H__