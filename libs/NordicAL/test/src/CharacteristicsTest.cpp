/**
 * @file	CharacteristicsTest.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief 	Run-time tests for aconno NordicAL Characteristic class
 * @version 0.1
 * @date 	2020-08-22
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------
#include "CharacteristicsTest.h"

#include <cstddef>

//--------------------------- STRUCTS AND ENUMS -------------------------------
Test::TestServer Test::TestServer::instance{};

//-------------------------------- CONSTANTS ----------------------------------
constexpr int8_t   testValue_int8        = -128;
constexpr uint8_t  testValue_uint8       = 255U;
constexpr int16_t  testValue_int16       = -32768;
constexpr uint16_t testValue_uint16      = 65535U;
constexpr int32_t  testValue_int32       = -2147483648;
constexpr uint64_t testValue_uint64      = 18446744073709551615U;
constexpr double   testValue_double      = -123.456;
constexpr char     testValue_char        = 'A';
constexpr uint8_t  testValue_updateValue = 101;

constexpr std::array<int8_t, 5> testValue_array_int8 = {-128, 127, 0, 64, -100};
constexpr std::array<int8_t, 5> testValue_wrong_array_int8 = {-127,
                                                              -127,
                                                              0,
                                                              64,
                                                              100};

constexpr std::array<uint16_t, 5> testValue_array_uint16 = {65535U,
                                                            0,
                                                            1,
                                                            32000,
                                                            100};

constexpr std::array<int32_t, 5> testValue_array_int32 = {-2147483648,
                                                          0,
                                                          2147483647,
                                                          64,
                                                          -64540};

constexpr std::array<double, 5> testValue_array_double = {0.0,
                                                          -123.45,
                                                          678.9,
                                                          64,
                                                          -100};

constexpr std::array<char, 5> testValue_array_char = {'B', 'u', 'n', 'n', 'y'};

constexpr std::array<uint16_t, 5> testValue_update_array_uint16 = {65535U,
                                                                   0,
                                                                   65535U,
                                                                   0,
                                                                   65535U};

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Create instance of eager-loading testing class.
 * 
 * @details	Creates both parent service for characteristics under test and
 * 			characteristics under test. 
 */
Test::TestServer::TestServer()
    : Test::Base("IO::BLE", "Characteristic"), testService(),
      basicEmptyChar(testService,
                     {.broadcast        = false,
                      .read             = false,
                      .writeNoResponse  = false,
                      .write            = false,
                      .notify           = false,
                      .indicate         = false,
                      .authSignedWrites = false},
                     0,
                     Config::baseUUID,
                     0xF00D),

      basicTypeChar_UINT8{testService,
                          {.broadcast        = false,
                           .read             = true,
                           .writeNoResponse  = false,
                           .write            = true,
                           .notify           = true,
                           .indicate         = false,
                           .authSignedWrites = false},
                          testValue_uint8},

      basicTypeChar_INT16(testService,
                          {.broadcast        = false,
                           .read             = true,
                           .writeNoResponse  = false,
                           .write            = true,
                           .notify           = true,
                           .indicate         = false,
                           .authSignedWrites = false},
                          testValue_int16),

      basicTypeChar_INT32(testService,
                          {.broadcast        = false,
                           .read             = true,
                           .writeNoResponse  = false,
                           .write            = true,
                           .notify           = true,
                           .indicate         = false,
                           .authSignedWrites = false},
                          testValue_int32),

      basicTypeChar_UINT64{testService,
                           {.broadcast        = false,
                            .read             = true,
                            .writeNoResponse  = false,
                            .write            = true,
                            .notify           = true,
                            .indicate         = false,
                            .authSignedWrites = false},
                           testValue_uint64},

      basicTypeChar_DOUBLE{testService,
                           {.broadcast        = false,
                            .read             = true,
                            .writeNoResponse  = false,
                            .write            = true,
                            .notify           = true,
                            .indicate         = false,
                            .authSignedWrites = false},
                           testValue_double},

      basicTypeChar_CHAR{testService,
                         {.broadcast        = false,
                          .read             = true,
                          .writeNoResponse  = false,
                          .write            = true,
                          .notify           = true,
                          .indicate         = false,
                          .authSignedWrites = false},
                         testValue_char},

      complexTypeChar_INT8{testService,
                           {.broadcast        = false,
                            .read             = true,
                            .writeNoResponse  = false,
                            .write            = true,
                            .notify           = true,
                            .indicate         = false,
                            .authSignedWrites = false},
                           testValue_array_int8},

      complexTypeChar_UINT16{testService,
                             {.broadcast        = false,
                              .read             = true,
                              .writeNoResponse  = false,
                              .write            = true,
                              .notify           = true,
                              .indicate         = false,
                              .authSignedWrites = false},
                             testValue_array_uint16},

      complexTypeChar_INT32{testService,
                            {.broadcast        = false,
                             .read             = true,
                             .writeNoResponse  = false,
                             .write            = true,
                             .notify           = true,
                             .indicate         = false,
                             .authSignedWrites = false},
                            testValue_array_int32},

      complexTypeChar_DOUBLE{testService,
                             {.broadcast        = false,
                              .read             = true,
                              .writeNoResponse  = false,
                              .write            = true,
                              .notify           = true,
                              .indicate         = false,
                              .authSignedWrites = false},
                             testValue_array_double},

      complexTypeChar_CHAR{testService,
                           {.broadcast        = false,
                            .read             = true,
                            .writeNoResponse  = false,
                            .write            = true,
                            .notify           = true,
                            .indicate         = false,
                            .authSignedWrites = false},
                           testValue_array_char},

      propertyChar_broadcast{testService,
                             {.broadcast        = true,
                              .read             = false,
                              .writeNoResponse  = false,
                              .write            = false,
                              .notify           = false,
                              .indicate         = false,
                              .authSignedWrites = false},
                             0},

      propertyChar_read{testService,
                        {.broadcast        = false,
                         .read             = true,
                         .writeNoResponse  = false,
                         .write            = false,
                         .notify           = false,
                         .indicate         = false,
                         .authSignedWrites = false},
                        0},

      propertyChar_writeCommand{testService,
                                {.broadcast        = false,
                                 .read             = false,
                                 .writeNoResponse  = true,
                                 .write            = false,
                                 .notify           = false,
                                 .indicate         = false,
                                 .authSignedWrites = false},
                                0},

      propertyChar_writeRequest{testService,
                                {.broadcast        = false,
                                 .read             = false,
                                 .writeNoResponse  = false,
                                 .write            = true,
                                 .notify           = false,
                                 .indicate         = false,
                                 .authSignedWrites = false},
                                0},

      propertyChar_notify{testService,
                          {.broadcast        = false,
                           .read             = false,
                           .writeNoResponse  = false,
                           .write            = false,
                           .notify           = true,
                           .indicate         = false,
                           .authSignedWrites = false},
                          0},

      propertyChar_indicate{testService,
                            {.broadcast        = false,
                             .read             = false,
                             .writeNoResponse  = false,
                             .write            = false,
                             .notify           = false,
                             .indicate         = true,
                             .authSignedWrites = false},
                            0},

      propertyChar_authSignedWrites{testService,
                                    {.broadcast        = false,
                                     .read             = false,
                                     .writeNoResponse  = false,
                                     .write            = false,
                                     .notify           = false,
                                     .indicate         = false,
                                     .authSignedWrites = true},
                                    0},

      propertyChar_writeConflict{testService,
                                 {.broadcast        = false,
                                  .read             = false,
                                  .writeNoResponse  = true,
                                  .write            = true,
                                  .notify           = false,
                                  .indicate         = false,
                                  .authSignedWrites = true},
                                 0},

      propertyChar_alertConflict{testService,
                                 {.broadcast        = false,
                                  .read             = false,
                                  .writeNoResponse  = false,
                                  .write            = false,
                                  .notify           = true,
                                  .indicate         = true,
                                  .authSignedWrites = true},
                                 0}
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Get test class singleton instance.
 */
Test::TestServer& Test::TestServer::getInstance()
{
    return instance;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------
/**
 * @brief Function which runs tests once system is initialized.
 */
void Test::TestServer::runInternal()
{
    /* --- Initial tests --- */
    assert(&basicEmptyChar.getService() ==
               &testService,
           "characteristic returns invalid parent service");

    /* --- */
    /* --- Initial value verification for basic types --- */
    assert(basicTypeChar_UINT8.getValue() == testValue_uint8,
           "characteristic uint8_t type returns invalid data value");

    assert(basicTypeChar_INT16.getValue() == testValue_int16,
           "characteristic int16_t type returns invalid data value");

    assert(basicTypeChar_INT32.getValue() == testValue_int32,
           "characteristic int32_t type returns invalid data value");

    assert(basicTypeChar_UINT64.getValue() == testValue_uint64,
           "characteristic uint64_t type returns invalid data value");

    assert(basicTypeChar_DOUBLE.getValue() == testValue_double,
           "characteristic double type returns invalid data value");

    assert(basicTypeChar_CHAR.getValue() == testValue_char,
           "characteristic char type returns invalid data value");

    /* --- */
    /* --- Verify basic type value update --- */
    basicTypeChar_UINT8.updateValue(testValue_updateValue);
    assert(basicTypeChar_UINT8.getValue() == testValue_updateValue,
           "characteristic uint8_t type value update operation failed");

    /* --- */
    /* --- Initial value verification for complex types --- */
    assert(complexTypeChar_INT8.getValue() == testValue_array_int8,
           "characteristic array type returns invalid data value");

    assert(complexTypeChar_INT8.getValue() != testValue_wrong_array_int8,
           "characteristic array type did not fail propery");

    assert(complexTypeChar_UINT16.getValue() == testValue_array_uint16,
           "characteristic array type returns invalid data value");

    assert(complexTypeChar_INT32.getValue() == testValue_array_int32,
           "characteristic array type returns invalid data value");

    assert(complexTypeChar_DOUBLE.getValue() == testValue_array_double,
           "characteristic array type returns invalid data value");

    assert(complexTypeChar_CHAR.getValue() == testValue_array_char,
           "characteristic array type returns invalid data value");

    /* --- */
    /* --- Verify complex type value update --- */
    complexTypeChar_UINT16.updateValue(testValue_update_array_uint16);
    assert(complexTypeChar_UINT16.getValue() == testValue_update_array_uint16,
           "characteristic array type value update operation failed");

    /* --- */
    /* --- Verify characteristic property options --- */
    // TODO: replace these checks with checks against property values read
    // directly from softdevice.
    assert(propertyChar_broadcast.getProperties().broadcast == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_read.getProperties().read == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_writeCommand.getProperties().writeNoResponse == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_writeRequest.getProperties().write == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_notify.getProperties().notify == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_indicate.getProperties().indicate == true,
           "creating characteristic with broadcast property failed");

    assert(propertyChar_authSignedWrites.getProperties().authSignedWrites ==
               true,
           "creating characteristic with broadcast property failed");

    /* --- */
    /* --- Verify handling of opposing characteristic property options --- */
    assert((propertyChar_writeConflict.getProperties().writeNoResponse ==
            false) &&
               (propertyChar_writeConflict.getProperties().write == true),
           "characteristic write properties conflict");

    assert((propertyChar_alertConflict.getProperties().notify == false) &&
               (propertyChar_alertConflict.getProperties().indicate == true),
           "characteristic alert properties conflict");
}

//---------------------------- STATIC FUNCTIONS -------------------------------
