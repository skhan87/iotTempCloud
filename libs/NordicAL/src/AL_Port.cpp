/**
 * @file AL_Nordic.cpp
 * @author Joshua Lauterbach (joshua@aconno.de)
 * @brief general nordic functions
 * @version 1.0
 * @date 2020-04-14
 *
 * @copyright aconno GmbH (c) 2020
 *
 */

//--------------------------------- INCLUDES ----------------------------------

#include "sdk_validation.h"  // this checks for missconfiguration of the SDK config

#include "AL_AnalogIn.h"
#include "AL_BLE.h"
#include "AL_Port.h"
#include "LoggerTask.h"
#include "AL_Log.h"
#include "Updater.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_sdh.h"
#include "nrf_sdh_freertos.h"
#include "hardfault.h"

#include <Advertiser.h>
#include <BLE_Utility.h>
#include <Error.h>
#include <FlashUtility.h>
#include <sdk_errors.h>

extern "C" {
#include "nrf_pwr_mgmt.h"  // this header lacks C++ extern protection
}

#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

#ifdef ENABLE_SYSVIEW_TRACE
#include "SystemView.h"
#endif

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------- PROTOTYPES ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

void Port::init()
{
#if NRF_LOG_ENABLED
    Log::Task::init();
#endif
    LOG_RAW_I("\n\nController Booted\n\n");

    // Activate deep sleep mode.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

// Set FPU to interrupt mode if used
#if (__FPU_USED == 1)
    NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOW);
    NVIC_ClearPendingIRQ(FPU_IRQn);
    NVIC_EnableIRQ(FPU_IRQn);
#endif

    // activate softdevice
    if (nrf_sdh_enable_request() != NRF_SUCCESS) {
        CHECK_ERROR(Error::Unknown);
    }

#ifdef ENABLE_SYSVIEW_TRACE
    Tools::SystemView::init();
#endif

    IO::BLE::Utility::init();

#if defined(ENABLE_OTA_UPDATES)
    nrf_pwr_mgmt_init();
    SYS::Updater::init();
#endif

    IO::Flash::Utility::init();

    // start softdevice task
    nrf_sdh_freertos_init(NULL, NULL);
}

/**
 * @brief overwrite default implementation, because log flush is at wrong
 * position in original implementation.
 * 
 * @details Overwrites weak standard implementation
 *
 * @param id
 * @param pc
 * @param info
 */
extern "C" void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    __disable_irq();
    switch (id) {
#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
        case NRF_FAULT_ID_SD_ASSERT:
            CHECK_ERROR(Error::Internal);
            break;
        case NRF_FAULT_ID_APP_MEMACC:
            CHECK_ERROR(Error::Memory);
            break;
#endif

#ifndef DEBUG
        case NRF_FAULT_ID_SDK_ASSERT: {
            CHECK_ERROR(Error::InvalidUse);
            break;
        }
        case NRF_FAULT_ID_SDK_ERROR: {
            CHECK_ERROR(Error::Unknown);
            break;
        }
#else
            // calling error function directly here for better info
        case NRF_FAULT_ID_SDK_ASSERT: {
            assert_info_t* p_info = (assert_info_t*)info;
            Error::internalCheck(
                Error::InvalidUse,
                p_info->line_num,
                reinterpret_cast<const char*>(p_info->p_file_name));
            break;
        }
        case NRF_FAULT_ID_SDK_ERROR: {
            error_info_t* p_info = (error_info_t*)info;
            Error::internalCheck(
                Error::Unknown,
                p_info->line_num,
                reinterpret_cast<const char*>(p_info->p_file_name));
            break;
        }
#endif
        default:
            CHECK_ERROR(Error::Unknown);
            break;
    }
}

/**
 * @brief Handles hard faults caught by nordic hardfault lib.
 * 
 * @details Replaces the nordic implementation for consistent error handling.
 * 
 * @param p_stack 
 */
extern "C" void HardFault_c_handler(uint32_t* p_stack_address)
{
    __disable_irq();
#if !(__CORTEX_M == 0x04)
#error unsupported
#endif

#ifndef CFSR_MMARVALID
#define CFSR_MMARVALID (1 << (0 + 7))
#endif

#ifndef CFSR_BFARVALID
#define CFSR_BFARVALID (1 << (8 + 7))
#endif

    HardFault_stack_t* p_stack     = (HardFault_stack_t*)p_stack_address;
    static const char* cfsr_msgs[] = {
        [0] = "The processor has attempted to execute an undefined instruction",
        [1] = "The processor attempted a load or store at a location that does "
              "not permit the operation",
        [2] = NULL,
        [3] = "Unstack for an exception return has caused one or more access "
              "violations",
        [4] = "Stacking for an exception entry has caused one or more access "
              "violations",
        [5] = "A MemManage fault occurred during floating-point lazy state "
              "preservation",
        [6] = NULL,
        [7] = NULL,
        [8] = "Instruction bus error",
        [9] = "Data bus error (PC value stacked for the exception return "
              "points to the instruction that caused the fault)",
        [10] = "Data bus error (return address in the stack frame is not "
               "related to the instruction that caused the error)",
        [11] = "Unstack for an exception return has caused one or more "
               "BusFaults",
        [12] = "Stacking for an exception entry has caused one or more "
               "BusFaults",
        [13] = "A bus fault occurred during floating-point lazy state "
               "preservation",
        [14] = NULL,
        [15] = NULL,
        [16] = "The processor has attempted to execute an undefined "
               "instruction",
        [17] = "The processor has attempted to execute an instruction that "
               "makes illegal use of the EPSR",
        [18] = "The processor has attempted an illegal load of EXC_RETURN to "
               "the PC, as a result of an invalid context, or an invalid "
               "EXC_RETURN value",
        [19] = "The processor has attempted to access a coprocessor",
        [20] = NULL,
        [21] = NULL,
        [22] = NULL,
        [23] = NULL,
        [24] = "The processor has made an unaligned memory access",
        [25] = "The processor has executed an SDIV or UDIV instruction with a "
               "divisor of 0",
    };

    uint32_t cfsr = SCB->CFSR;

    if (p_stack != NULL) {
        // Print information about error.
        NRF_LOG_ERROR("HARD FAULT at 0x%08X", p_stack->pc);
        NRF_LOG_ERROR("  R0:  0x%08X  R1:  0x%08X  R2:  0x%08X  R3:  0x%08X",
                      p_stack->r0,
                      p_stack->r1,
                      p_stack->r2,
                      p_stack->r3);
        NRF_LOG_ERROR("  R12: 0x%08X  LR:  0x%08X  PSR: 0x%08X",
                      p_stack->r12,
                      p_stack->lr,
                      p_stack->psr);
    } else {
        CHECK_ERROR(Error::StackCorrupt);
    }

    if (SCB->HFSR & SCB_HFSR_VECTTBL_Msk) {
        NRF_LOG_ERROR("Cause: BusFault on a vector table read during exception "
                      "processing.");
    }

    for (uint32_t i = 0; i < sizeof(cfsr_msgs) / sizeof(cfsr_msgs[0]); i++) {
        if (((cfsr & (1 << i)) != 0) && (cfsr_msgs[i] != NULL)) {
            NRF_LOG_ERROR("Cause: %s.", (uint32_t)cfsr_msgs[i]);
        }
    }

    if (cfsr & CFSR_MMARVALID) {
        NRF_LOG_ERROR("MemManage Fault Address: 0x%08X", SCB->MMFAR);
    }

    if (cfsr & CFSR_BFARVALID) {
        NRF_LOG_ERROR("Bus Fault Address: 0x%08X", SCB->BFAR);
    }

    CHECK_ERROR(Error::Unknown);
}

#if (__FPU_USED == 1)
/**
 * @brief Handles interrupt driven FPU operation.
 * 
 * @details Function handles and clears exception flags in FPSCR register and at the stack.
 * During interrupt, handler execution FPU registers might be copied to the stack
 * (see lazy stacking option) and it is necessary to clear data at the stack
 * which will be recovered in the return from interrupt handling.
 * 
 */
extern "C" void FPU_IRQHandler(void)
{
    // Prepare pointer to stack address with pushed FPSCR register (0x40 is FPSCR register offset in stacked data)
    uint32_t* fpscr = (uint32_t*)(FPU->FPCAR + 0x40);
    // Execute FPU instruction to activate lazy stacking
    (void)__get_FPSCR();
    // TODO: Check exception flags

    // Clear flags in stacked FPSCR register. To clear IDC, IXC, UFC, OFC, DZC and IOC flags, use 0x0000009F mask.
    *fpscr = *fpscr & ~(0x0000009F);
}
#endif

//---------------------------- STATIC FUNCTIONS -------------------------------
