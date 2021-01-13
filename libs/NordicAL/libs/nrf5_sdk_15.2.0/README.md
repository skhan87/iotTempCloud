# Nordic software development kit, version 15.2.0

Repository for original version of Nordic nRF5 SDK 15.2.0.

Any modifications done by aconno developers will be listed below.

Set up Linux GCC compiler location and version

Before first use, SDK requires for user to specify which compiler to use for which platform.
This is done by modifing values in appropriate file inside nRF5_SDK_15.2.0/components/toolchain/ .
At aconno, we use GCC compiler and Linux platform, so we edit Makefile.posix file inside gcc/ directory.
Concretely, we use GNU ARM embedded toolchain version 8.3.1, and compiler folder is registered in system path.
For more details or instruction on how to add this toolchain on a system that already uses a different GCC version
use the following guide:

https://kb.zubax.com/display/MAINKB/GNU+ARM+embedded+toolchain

After correct setup, contents of your Makefile.posix file should be:

	GNU_INSTALL_ROOT ?=
	GNU_VERSION ?= 8.3.1
	GNU_PREFIX ?= arm-none-eabi


Fix increased beacon power consumption

At some point Nordic introduced a fix for excessive power consumption. Ironically, this fix increased power consumption
in some cases. This results in higher power consumption of aconno beacons. To fix this, we undo Nordic fix.
More details available here:

	https://devzone.nordicsemi.com/f/nordic-q-a/35073/high-power-consumption-of-softdevice
	

## Using NFC, SWO and trace data pins as GPIOs

In the Nordic SDK (specifically in the system_nrf52.c) there are defines 
which should be defined or undefined in order to use aformentioned pins as common GPIOs.


 - ENABLE_SWO - converts the gpio pin to the single wire output pin

 - ENABLE_TRACE - converts the gpio pins to the trace data pins


Macro values above should be undefined or erased so that the system initialization function (SystemInit(void)) can properly configure the pins. Although left undefined in the project itself, macros seem to be enabled during runtime, taking over the GPIO pins. Likely culprit is Segger JLink, which may automatically enable SWO pin via the above macros. As we do not need SWO pin, removing this code section is acceptable.


In case the NFC pins should be used as GPIOs the following macro should be defined:

 - CONFIG_NFCT_PINS_AS_GPIOS
 
 
 Also it is possible to enable Reset pin by defining:
 
 - CONFIG_GPIO_AS_PINRESET


## 27.11.2020

Major changes to add SDK as submodule in the NordicAL

*  Remove examples to save space.
*  Delete FreeRTOS files except portable. The FreeRTOS core will be maintained in its own module.
*  Include sdk_validation.h and add nrfx_common.h to it. This validates the settings made in sdk_config.h

## 01.12.2020

Changes introduced to fix OTA key and update generation scripts after SDK NordicAL submodule integration

* Reintroduce DFU secure bootloader. Originally found in /SDK/examples/dfu, this folder is now relocated to
  /SDK/components/dfu. Out of many previously-contained implementations, only bootloader implementations 
  relevant for use with BLE on nrf52832 and nrf52840 platforms are added. 
* micro-ecc added as SDK git submodule. micro-ecc library is required to build secure bootloader. Therefore
  it is logical to contain it in the project.
