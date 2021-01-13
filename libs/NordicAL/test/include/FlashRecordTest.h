/**
 * @file RecordTest.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief test for a flash record
 * @version 1.0
 * @date 2020-04-07
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __RecordTEST_H__
#define __RecordTEST_H__

//-------------------------------- PROTOTYPES ---------------------------------

namespace Test::IOFlash
{
class Record;
}

//--------------------------------- INCLUDES ----------------------------------

#include <AL_FlashRecord.h>
#include <TestBase.h>

namespace Test::IOFlash
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------

/**
 * @brief test for a flash record
 */
class Record : public Test::Base {
    // constructors
public:
    // delete default constructors
    Record(const Record& other) = delete;
    Record& operator=(const Record& other) = delete;

    /**
     * @brief get singleton instance
     */
    static Record& getInstance();

private:
    Record();

    virtual void runInternal() final;

    IO::Flash::File             file;
    IO::Flash::Record<uint32_t> n1;
    IO::Flash::Record<int16_t>  n2;

    /** singleton instance */
    static Record instance;
};
}  // namespace Test::IOFlash
#endif  //__RecordTEST_H__