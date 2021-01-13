/**
 * @file 	sysview_softdevice.c
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief	Implements interface between SystemView trace analyzer and Nordic Softdevice BLE stack
 * @details Based on RKs work found here:
 *          https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/instrumenting-with-segger-systemview
 * 
 *          Part of the unofficial SystemView Softdevice patch.
 *          This patch does not guarantee stability, and is unmaintained.
 *          However, it does provide one with a very detailed view into Softdevice functionality
 *          at runtime, making it worth the downsides when debugging BLE bugs.
 * 
 *          Majority of functions declared here are contained within closed-source static library
 *          described in a blog post on the link above. 
 *
 * @version	1.0.0
 * @date 	2020-10-22
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

#include "sysview_softdevice.h"
#include "SysView_Logged_Events_Config.h"
#include "nrf52.h"

/**
 * @brief Enable logs according to user configuration
 * 
 * @details Disables all logs, then reenables those turned on in the logged
 *          events config file.
 * 
 *          User configuration header can be found in config directory or the
 *          repository.
 * 
 */
void sysview_enable_configured_logs()
{
    // Disable all logging
    sysview_all_irq_log(false);

    // Enable logging of all IRQs that user enabled in the config file
#if SV_LOG_Reset_IRQn
    sysview_irq_log(Reset_IRQn, true);
#endif

#if SV_LOG_NonMaskableInt_IRQn
    sysview_irq_log(NonMaskableInt_IRQn, true);
#endif

#if SV_LOG_HardFault_IRQn
    sysview_irq_log(HardFault_IRQn, true);
#endif

#if SV_LOG_MemoryManagement_IRQn
    sysview_irq_log(MemoryManagement_IRQn, true);
#endif

#if SV_LOG_BusFault_IRQn
    sysview_irq_log(BusFault_IRQn, true);
#endif

#if SV_LOG_UsageFault_IRQn
    sysview_irq_log(UsageFault_IRQn, true);
#endif

#if SV_LOG_SVCall_IRQn
    sysview_irq_log(SVCall_IRQn, true);
#endif

#if SV_LOG_DebugMonitor_IRQn
    sysview_irq_log(DebugMonitor_IRQn, true);
#endif

#if SV_LOG_PendSV_IRQn
    sysview_irq_log(PendSV_IRQn, true);
#endif

#if SV_LOG_SysTick_IRQn
    sysview_irq_log(SysTick_IRQn, true);
#endif

#if SV_LOG_POWER_CLOCK_IRQn
    sysview_irq_log(POWER_CLOCK_IRQn, true);
#endif

#if SV_LOG_RADIO_IRQn
    sysview_irq_log(RADIO_IRQn, true);
#endif

#if SV_LOG_UARTE0_IRQn
    sysview_irq_log(UARTE0_UART0_IRQn, true);
#endif

#if SV_LOG_SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
    sysview_irq_log(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, true);
#endif

#if SV_LOG_SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
    sysview_irq_log(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, true);
#endif

#if SV_LOG_NFCT_IRQn
    sysview_irq_log(NFCT_IRQn, true);
#endif

#if SV_LOG_GPIOTE_IRQn
    sysview_irq_log(GPIOTE_IRQn, true);
#endif

#if SV_LOG_SAADC_IRQn
    sysview_irq_log(SAADC_IRQn, true);
#endif

#if SV_LOG_TIMER0_IRQn
    sysview_irq_log(TIMER0_IRQn, true);
#endif

#if SV_LOG_TIMER1_IRQn
    sysview_irq_log(TIMER1_IRQn, true);
#endif

#if SV_LOG_TIMER2_IRQn
    sysview_irq_log(TIMER2_IRQn, true);
#endif

#if SV_LOG_RTC0_IRQn
    sysview_irq_log(RTC0_IRQn, true);
#endif

#if SV_LOG_TEMP_IRQn
    sysview_irq_log(TEMP_IRQn, true);
#endif

#if SV_LOG_RNG_IRQn
    sysview_irq_log(RNG_IRQn, true);
#endif

#if SV_LOG_ECB_IRQn
    sysview_irq_log(ECB_IRQn, true);
#endif

#if SV_LOG_CCM_AAR_IRQn
    sysview_irq_log(CCM_AAR_IRQn, true);
#endif

#if SV_LOG_WDT_IRQn
    sysview_irq_log(WDT_IRQn, true);
#endif

#if SV_LOG_RTC1_IRQn
    sysview_irq_log(RTC1_IRQn, true);
#endif

#if SV_LOG_QDEC_IRQn
    sysview_irq_log(QDEC_IRQn, true);
#endif

#if SV_LOG_COMP_LPCOMP_IRQn
    sysview_irq_log(COMP_LPCOMP_IRQn, true);
#endif

#if SV_LOG_SWI0_EGU0_IRQn
    sysview_irq_log(SWI0_EGU0_IRQn, true);
#endif

#if SV_LOG_SWI1_EGU1_IRQn
    sysview_irq_log(SWI1_EGU1_IRQn, true);
#endif

#if SV_LOG_SWI2_EGU2_IRQn
    sysview_irq_log(SWI2_EGU2_IRQn, true);
#endif

#if SV_LOG_SWI3_EGU3_IRQn
    sysview_irq_log(SWI3_EGU3_IRQn, true);
#endif

#if SV_LOG_SWI4_EGU4_IRQn
    sysview_irq_log(SWI4_EGU4_IRQn, true);
#endif

#if SV_LOG_SWI5_EGU5_IRQn
    sysview_irq_log(SWI5_EGU5_IRQn, true);
#endif

#if SV_LOG_PWM0_IRQn
    sysview_irq_log(PWM0_IRQn, true);
#endif

#if SV_LOG_PDM_IRQn
    sysview_irq_log(PDM_IRQn, true);
#endif

#if SV_LOG_MWU_IRQn
    sysview_irq_log(MWU_IRQn, true);
#endif

#if SV_LOG_PWM1_IRQn
    sysview_irq_log(PWM1_IRQn, true);
#endif

#if SV_LOG_PWM2_IRQn
    sysview_irq_log(PWM2_IRQn, true);
#endif

#if SV_LOG_SPIM2_SPIS2_SPI2_IRQn
    sysview_irq_log(SPIM2_SPIS2_SPI2_IRQn, true);
#endif

#if SV_LOG_RTC2_IRQn
    sysview_irq_log(RTC2_IRQn, true);
#endif

#if SV_LOG_I2S_IRQn
    sysview_irq_log(I2S_IRQn, true);
#endif

#if SV_LOG_FPU_IRQn
    sysview_irq_log(FPU_IRQn, true);
#endif
}