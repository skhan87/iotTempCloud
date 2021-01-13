/**
 * @file 	sysview_softdevice.h
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

#ifndef __SYSVIEW_SOFTDEVICE_H__
#define __SYSVIEW_SOFTDEVICE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable or disable one single IRQ for logging
 * 
 * @example 
 *      sysview_irq_log( SysTick_IRQn, false );		// disable SysTick logging
 *      sysview_irq_log( RTC1_IRQn, true );			// enable RTC1 logging
 *      sysview_irq_log( SVCALL_IRQn, true );		// you probably want this
 */
void sysview_irq_log(int8_t irq_num, bool enable);

/**
 * @brief Enable or disable logging of all IRQs
 * 
 * @details Equivalent to calling 'sysview_irq_log( <irq>, enable )' for all 
 *          IRQ numbers.
 * 
 *          Default state is enabled.
 * 
 */
void sysview_all_irq_log(bool enable);

/**
 * @brief Enable logs according to user configuration
 * 
 * @details Disables all logs, then reenables those turned on in the logged
 *          events config file.
 * 
 */
void sysview_enable_configured_logs();

/**
 * @brief Enable or disable softdevice tracing
 * 
 * @param enabled   true to turn on, false to turn off
 * @param tasks     true to artificially group events into User/Event/Softdevice
 *                  tasks, false to just output the events
 * 
 */
void sysview_softdevice_enable(bool enabled, bool tasks);

/**
 * @brief Manual logging of in and out of idle mode
 * 
 */
void sysview_softdevice_mark_sleep(bool sleeping);

#endif  // __SYSVIEW_SOFTDEVICE_H__

#ifdef __cplusplus
}
#endif