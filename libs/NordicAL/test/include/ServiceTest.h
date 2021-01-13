/**
 * @file	ServiceTest.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief 	Run-time tests for aconno NordicAL Service class
 * @version 0.1
 * @date 	2020-08-22
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __SERVICETEST_H__
#define __SERVICETEST_H__

//-------------------------------- PROTOTYPES ---------------------------------
namespace Test
{
class TestServices;
}

//--------------------------------- INCLUDES ----------------------------------
#include "AL_Service.h"
#include "TestBase.h"

#include <cstdint>

namespace Test
{
//-------------------------------- CONSTANTS ----------------------------------

//---------------------------- CLASS DEFINITION -------------------------------
class TestServices : public Test::Base {
public:
    // Delete default constructors
    TestServices(const TestServices& other) = delete;
    TestServices& operator=(const TestServices& other) = delete;

    /* --- Public API --- */
    static TestServices& getInstance();

private:
    /* --- Constructor --- */
    TestServices();

    /* --- Private API --- */
    virtual void runInternal() final;

    /* --- Private members --- */
    IO::BLE::Service basicService;
    IO::BLE::Service extendedService;

    static TestServices instance; /**< Singleton instance */
};

}  // namespace Test
#endif  //__SERVICETEST_H__