/**
 * @file 	SysView_Logged_Events_Config.h
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief	User configuration for application-and-sd mode data logging
 * 
 * @details Set any of the below options to '1' to see its events inside
 *          SystemView trace. Note that some options are inter-dependant, such
 *          as SVCall_IRQn and RADIO_IRQn - without SVCall enabled, RADIO does
 *          not output.
 * 
 *          Also, freeRTOS must run on RTC rather than use systick when using
 *          trace analyzer. Consequently, application-and-sd mode outputs
 *          all events of RTC1 irregardless of the option selected below.
 * 
 *          Many of the options below were never tested, and are given to users
 *          as-is. Nonetheless, author has not came across significant issues.
 *
 * @version	1.0.0
 * @date 	2020-10-22
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

// clang-format off
#ifndef __SYSVIEW_LOGGED_EVENTS_CONFIG_H__
#define __SYSVIEW_LOGGED_EVENTS_CONFIG_H__

/* =======================================  ARM Cortex-M4 Specific Interrupts  ===============================================      */
  #define SV_LOG_Reset_IRQn                 0   /*!< Reset Vector, invoked on Power up and warm reset                               */
  #define SV_LOG_NonMaskableInt_IRQn        0   /*!< Non maskable Interrupt, cannot be stopped or preempted                         */
  #define SV_LOG_HardFault_IRQn             0   /*!< Hard Fault, all classes of Fault                                               */
  #define SV_LOG_MemoryManagement_IRQn      0   /*!< Memory Management, MPU mismatch, including Access Violation and No Match       */
  #define SV_LOG_BusFault_IRQn              0   /*!< Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory related Fault */
  #define SV_LOG_UsageFault_IRQn            0   /*!< Usage Fault, i.e. Undef Instruction, Illegal State Transition                  */
  #define SV_LOG_SVCall_IRQn                1   /*!< System Service Call via SVC instruction                                        */
  #define SV_LOG_DebugMonitor_IRQn          0   /*!< Debug Monitor                                                                  */
  #define SV_LOG_PendSV_IRQn                0   /*!< Pendable request for system service                                            */
  #define SV_LOG_SysTick_IRQn               0   /*!< System Tick Timer                                                              */

/* ==========================================  nrf52 Specific Interrupts  =================================================         */
  #define SV_LOG_POWER_CLOCK_IRQn           0   /*!< POWER_CLOCK                                                                    */
  #define SV_LOG_RADIO_IRQn                 1   /*!< RADIO                                                                          */
  #define SV_LOG_UARTE0_UART0_IRQn          0   /*!< UARTE0                                                                         */
  #define SV_LOG_NFCT_IRQn                  0   /*!< NFCT                                                                           */
  #define SV_LOG_GPIOTE_IRQn                0   /*!< GPIOTE                                                                         */
  #define SV_LOG_SAADC_IRQn                 0   /*!< SAADC                                                                          */
  #define SV_LOG_TIMER0_IRQn                0   /*!< TIMER0                                                                         */
  #define SV_LOG_TIMER1_IRQn                0   /*!< TIMER1                                                                         */
  #define SV_LOG_TIMER2_IRQn                0   /*!< TIMER2                                                                         */
  #define SV_LOG_RTC0_IRQn                  0   /*!< RTC0                                                                           */
  #define SV_LOG_TEMP_IRQn                  0   /*!< TEMP                                                                           */
  #define SV_LOG_RNG_IRQn                   0   /*!< RNG                                                                            */
  #define SV_LOG_ECB_IRQn                   0   /*!< ECB                                                                            */
  #define SV_LOG_CCM_AAR_IRQn               0   /*!< CCM_AAR                                                                        */
  #define SV_LOG_WDT_IRQn                   0   /*!< WDT                                                                            */
  #define SV_LOG_RTC1_IRQn                  1   /*!< RTC1                                                                           */
  #define SV_LOG_QDEC_IRQn                  0   /*!< QDEC                                                                           */
  #define SV_LOG_COMP_LPCOMP_IRQn           0   /*!< COMP_LPCOMP                                                                    */
  #define SV_LOG_SWI0_EGU0_IRQn             0   /*!< SWI0_EGU0                                                                      */
  #define SV_LOG_SWI1_EGU1_IRQn             0   /*!< SWI1_EGU1                                                                      */
  #define SV_LOG_SWI2_EGU2_IRQn             1   /*!< SWI2_EGU2                                                                      */
  #define SV_LOG_SWI3_EGU3_IRQn             0   /*!< SWI3_EGU3                                                                      */
  #define SV_LOG_SWI4_EGU4_IRQn             0   /*!< SWI4_EGU4                                                                      */
  #define SV_LOG_SWI5_EGU5_IRQn             0   /*!< SWI5_EGU5                                                                      */
  #define SV_LOG_PWM0_IRQn                  0   /*!< PWM0                                                                           */
  #define SV_LOG_PDM_IRQn                   0   /*!< PDM                                                                            */
  #define SV_LOG_MWU_IRQn                   0   /*!< MWU                                                                            */
  #define SV_LOG_PWM1_IRQn                  0   /*!< PWM1                                                                           */
  #define SV_LOG_PWM2_IRQn                  0   /*!< PWM2                                                                           */
  #define SV_LOG_SPIM2_SPIS2_SPI2_IRQn      0   /*!< SPIM2_SPIS2_SPI2                                                               */
  #define SV_LOG_RTC2_IRQn                  0   /*!< RTC2                                                                           */
  #define SV_LOG_I2S_IRQn                   0   /*!< I2S                                                                            */
  #define SV_LOG_FPU_IRQn                   0   /*!< FPU                                                                            */
  #define SV_LOG_SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn 0  /*!< SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0                                   */
  #define SV_LOG_SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn 0  /*!< SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1                                   */

#endif  // __SYSVIEW_LOGGED_EVENTS_CONFIG_H__
// clang-format on
