/**
 * @file	ServiceTest.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief 	Run-time tests for aconno NordicAL Service class
 * @version 0.1
 * @date 	2020-08-22
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------
#include "ServiceTest.h"

#include <cstddef>

//--------------------------- STRUCTS AND ENUMS -------------------------------
Test::TestServices Test::TestServices::instance{};

//-------------------------------- CONSTANTS ----------------------------------
constexpr std::array<uint8_t, 16> testCustomBaseUUID    = {0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0,
                                                    0xA0};
uint16_t                      testCustomServiceUUID = 0xABBA;

//------------------------------ CONSTRUCTOR ----------------------------------
/**
 * @brief Create instance of eager-loading testing class.
 */
Test::TestServices::TestServices()
    : Test::Base("IO::BLE", "Service"), basicService(),
      extendedService(testCustomBaseUUID, testCustomServiceUUID)
{}

//--------------------------- EXPOSED FUNCTIONS -------------------------------
/**
 * @brief Get test class singleton instance.
 */
Test::TestServices& Test::TestServices::getInstance()
{
    return instance;
}

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------

//--------------------------- PRIVATE FUNCTIONS -------------------------------
/**
 * @brief Function which runs tests once system is initialized.
 */
void Test::TestServices::runInternal()
{
    bool serviceFound = false;

    for (auto& service : IO::BLE::Service::getList()) {
        if (service.getServiceUUID().uuid ==
            basicService.getServiceUUID().uuid) {
            serviceFound = true;
        }
    }

    assert(serviceFound == true, "basic service initialization failed.");

    serviceFound = false;

    for (auto& service : IO::BLE::Service::getList()) {
        if (service.getServiceUUID().uuid ==
            extendedService.getServiceUUID().uuid) {
            serviceFound = true;
        }
    }

    assert(serviceFound == true, "extended service initialization failed.");
}

//---------------------------- STATIC FUNCTIONS -------------------------------
