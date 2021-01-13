/**
 * @file AL_Port.h
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief general nordic AL functions
 * @version 1.0
 * @date 2020-04-14
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

#ifndef __AL_NORDIC_H__
#define __AL_NORDIC_H__

//-------------------------------- PROTOTYPES ---------------------------------

//--------------------------------- INCLUDES ----------------------------------

extern "C" void
                app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info);
extern "C" void HardFault_c_handler(uint32_t* p_stack_address);
extern "C" void FPU_IRQHandler(void);

#include <cstdint>

namespace Port
{
//-------------------------------- CONSTANTS ----------------------------------

//-------------------------------- FUNCTIONS ----------------------------------

/**
 * @brief initializes all Nordic libraries
 */
void init();

}  // namespace Port
#endif  //__AL_NORDIC_H__