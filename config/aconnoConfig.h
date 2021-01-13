/**
 * @file aconnoConfig.h
 * @author Jurica Resetar (jurica@aconno.de)
 * @author Dominik Bartolovic (dominik@aconno.de)
 * @brief
 * @version 0.1
 * @date 2019-01-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __ACONNO_CONFIG_H__
#define __ACONNO_CONFIG_H__

#include <AL_BLE.h>
#include <cstdint>

namespace Config
{
/** Aconno ID universal to all projects */
constexpr uint8_t ACONNO_ID = 0x69;
/** device name */
constexpr const char* const DEVICE_NAME = "ClinarisBeacon";
constexpr std::array<uint8_t, 16> baseUUID = {0xc8,
                                              0x97,
                                              0xc7,
                                              0xf2,
                                              0x20,
                                              0xfd,
                                              0x43,
                                              0xed,
                                              0xaa,
                                              0x9d,
                                              0xd0,
                                              0xa4,
                                              0x3e,
                                              0x9b,
                                              0x7c,
                                              0x8e};

constexpr uint16_t BLE_CONNECTABLE_ADV_INTERVAL = 500;
}  // namespace Config

#endif  // __ACONNO_CONFIG_H__
