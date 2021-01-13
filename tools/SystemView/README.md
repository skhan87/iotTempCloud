This repository contains all files necessary to perform trace analysis on aconno beacons based
on nRF52832 and nRF52840 chips using Seggers SystemView trace analyzer tool.

 Implements trace analysis using Segger SystemView tool.

 SystemView is a trace analyzer tools from Segger which utilizes Seggers RTT capabilities to provide insight into events and workflow of embedded system during runtime.

 All computing system rely on time-dependant utilities such as interrupts, timers, tasks, semaphores and other to perform their job. In any such system, and most intensly in RTOS system, timing of such events is crucial for correct operation. Yet it is very difficult to assemble many time-dependant object into a single program with certanty that all timings are correct and no errors are caused by improperly set priorities or ill-timed context switches. Issues and bugs love nesting into timing-related problems.

 Debugging timing-related issues used to be black magic, a mix of assumptions, luck and guessing. However, as time went by our tools for combating such issues improved. Trace analyzers use data, recorded during runtime of a program on the actual hardware platform in least intrusive way possible, to present the programmer with tools needed to get deeper insight into program runtime behavior.

 SystemView is one of such tools. This module main function is to integrate tracing capability into aconno firmware framework, built upon Nordic platforms. It should be added to aconno projectsas git submodule, after which it integrates itself into project build. Tracing capability is achieved mainly by adding libraries which allow for recording CPU events (Seggers RTT library), libraries which integrate SystemView functionality and libraries
 which provide freeRTOS awareness to the trace.

 Once added to project build, tracing is activated by the user on the command line, using make command line variable ENABLE_TRACE. Two modes of tracing are implemented: application-only tracing and application-and-softdevice tracing.
     
     * application-only
             - records events on the application level
                 (including freeRTOS events)
             - natively supported by Segger
             - stable
             - lacks softdevice awareness
     
     * application-and-softdevice
             - records all user-selected events, including all ISR
                 events, softdevice events and freeRTOS events
             - implemented as a "patch", not natively supported by
                 Segger or by Nordic
             - closed-source
             - may crash, does not display task names
             - implements full softdevice awareness
Main difference between the two modes is in softdevice awareness. Application-only mode is not aware that softdevice is running on the platform. Therefore, if taskA is preempted by softdevice to process a radio event for 2 ms and then returns, SystemView will incorrectly add those 2 ms to the runtime of taskA. Therefore, timing of the tasks in this mode are not to be trusted completely.

Application-and-softdevice mode offer much more information and is much more accurate. However, this comes with a price. Neither Nordic nor Segger maintain this functionality. Rather, it was developed by a curious individual, using knowledge and assumptions on inner workings of proprietary Softdevice stack, and provided to the community as a static library with no access to source code (due to proprietary nature of Softdevice). As such, patching the trace analysis with it may result with occasional crashes of tracing software and other issues and bugs reported by the author himself.

There is not other way, unfortunately, to obtain the same level of insight into the platform using this or any other tracing tool, as softdevice is closed source and provides no hooks to monitor its inner workings. 

Most users will want to use the application-only mode. This will give them a great insight into the workflow of their application code, which aids debugging tremendiously.

Application-and-softdevice mode is mainly reserved for developers building new libraries directly upon BLE stack. Flawed as it may be, this mode will still provide them with useful information about what is going on beneath the softdevice hood, and make catching of really hard timing bugs easier.

This module utilizes Segger SystemView v320. It is downloadable for free from Seggers webpage. Once downloaded, flash your platform using proper make terminal command, run freshly installed SystemView program and press 'Play' sign to start streaming data.

For detailed instructions on how to use SystemView, check the manual in docs/ directory of this module.

To use either of the modes, give proper value to the make command line variable:
         
         make nrf52832_debug ENABLE_TRACE=APP
         
         or
         
         make nrf52840_release ENABLE_TRACE=APP_WITH_SD upload


