/**
 * @file	CharacteristicsTest.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief 	Run-time tests for aconno NordicAL Characteristic class
 * @version 0.1
 * @date 	2020-08-22
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __CHARACTERISTICSTEST_H__
#define __CHARACTERISTICSTEST_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace Test
{
class TestServer;
}

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Characteristic.h"
#include "AL_Service.h"
#include "TestBase.h"

#include <cstdint>

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class TestServer : public Test::Base {
public:
    // Delete default constructors
    TestServer(const TestServer& other) = delete;
    TestServer& operator=(const TestServer& other) = delete;

    /* --- Public API --- */
    static TestServer& getInstance();

private:
    /* --- Constructor --- */
    TestServer();

    /* --- Private API --- */
    virtual void runInternal() final;

    /* --- Private members --- */
    IO::BLE::Service                  testService;
    IO::BLE::Characteristic<uint8_t>  basicEmptyChar;
    IO::BLE::Characteristic<uint8_t>  basicTypeChar_UINT8;
    IO::BLE::Characteristic<int16_t>  basicTypeChar_INT16;
    IO::BLE::Characteristic<int32_t>  basicTypeChar_INT32;
    IO::BLE::Characteristic<uint64_t> basicTypeChar_UINT64;
    IO::BLE::Characteristic<double>   basicTypeChar_DOUBLE;
    IO::BLE::Characteristic<char>     basicTypeChar_CHAR;

    IO::BLE::Characteristic<std::array<int8_t, 5>>   complexTypeChar_INT8;
    IO::BLE::Characteristic<std::array<uint16_t, 5>> complexTypeChar_UINT16;
    IO::BLE::Characteristic<std::array<int32_t, 5>>  complexTypeChar_INT32;
    IO::BLE::Characteristic<std::array<double, 5>>   complexTypeChar_DOUBLE;
    IO::BLE::Characteristic<std::array<char, 5>>     complexTypeChar_CHAR;

    IO::BLE::Characteristic<uint8_t> propertyChar_broadcast;
    IO::BLE::Characteristic<uint8_t> propertyChar_read;
    IO::BLE::Characteristic<uint8_t> propertyChar_writeCommand;
    IO::BLE::Characteristic<uint8_t> propertyChar_writeRequest;
    IO::BLE::Characteristic<uint8_t> propertyChar_notify;
    IO::BLE::Characteristic<uint8_t> propertyChar_indicate;
    IO::BLE::Characteristic<uint8_t> propertyChar_authSignedWrites;
    IO::BLE::Characteristic<uint8_t> propertyChar_writeConflict;
    IO::BLE::Characteristic<uint8_t> propertyChar_alertConflict;

    static TestServer instance; /**< Singleton instance */
};

}  // namespace Test
#endif  //__CHARACTERISTICSTEST_H__