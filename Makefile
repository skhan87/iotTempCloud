# Main project makefile.
#
# aconno projects use makefiles to configure, compile and flash aconno
# firmware. aconno makefiles are organized as three-layer structure:
# 		
# 		project makefile
# 				|
# 				aconno common makefile
# 							|
# 							platform A specific makefile
# 							platform B specific makefile
# 							...
#
# - project makefile - found in project root directory, this makefile
#		defines project-specific settings and passes user command-line
#		arguments to lower layer makefiles
# 
# - aconno common makefile - located in build submodule directory, this
# 		makefile contains all features and options that are shared between
#		all aconno firmware platforms, such as Nordic SDK file includes
#		and defines, compiler flags, sanity checks and common targets
#
# - platform-specific makefiles - each hardware platform supported by
#		aconno firmware has unique makefile which contains all files,
#		compiler flags, configuration options and targets that are
#		unique to that chipset
#
# Using the commandline interface programmers can perform project build, upload 
# firmware to hardware platform, set SDK configuration options and do other
# tasks related to the project. User commands are passed on to proper sub-makefiles
# which perform the actual action.
#
# When setting up new aconno project, this makefile should serve as a 
# template. All project-specific configuration options should be set in this makefile,
# without needing to dig deeper into sub-makefiles.
#
#
# Usage help
# --------------------------------------------------------------------
# This makefile is only defining project-related files and flags,
# and then passing on the received commands to sub-makefiles.
#
# Example usage:
#
# make nrf52832 			(builds)
# make nrf52832 upload 		(builds and uploads)
# make nrf52832 sdk_config 	(starts SDK configuration tool)
#
# For further help, type 'make help' on the command line.
#
# aconno d.o.o.
# Author: Hrvoje Brezak (hrvoje@aconno.de)
#

#### Project makefile header ########################################################
# Must be included on top of project makefile! Do not edit.

# This line exports all defined variables to sub-makefiles
export 

# Obtain absolute path to project.
PROJ_DIR := $(strip $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
#####################################################################################



# --------------------------------------------------------------------
# Project details
# --------------------------------------------------------------------
PROJECT_NAME     := iotTempCloud


# --------------------------------------------------------------------
# Project options
# --------------------------------------------------------------------
SDK_VERSION := nRF5_SDK_15.2.0
SDK_ROOT := $(PROJ_DIR)/libs/NordicAL/libs/nrf5_sdk_15.2.0/
CUSTOM_OUTPUT_DIRECTORY :=

# --------------------------------------------------------------------
# Project defines
# --------------------------------------------------------------------

ENABLE_OTA_UPDATES   := YES

# --------------------------------------------------------------------
# Project files
# --------------------------------------------------------------------
# Include any files that are uniquely required for this project

# Include project-specific source files
PROJ_SRC += \
  $(PROJ_DIR)/source/main.cpp                 \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_freertos.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_pwm.c \
  $(PROJ_DIR)/libs/NordicAL/modules/BLE/standardServices/src/AL_BatteryService.cpp \

# Include project-specific headers
PROJ_INC += \
  $(PROJ_DIR)/config                                    \
  $(SDK_ROOT)/components/libraries/atomic_flags         \
  $(SDK_ROOT)/components/libraries/timer                \
  $(PROJ_DIR)/libs/NordicAL/modules/BLE/include         \
  $(PROJ_DIR)/libs/NordicAL/modules/Serial/I2C/include  \

# Include project-specific library files
PROJ_LIB_FILES += \

# --------------------------------------------------------------------
# Project compiler flags
# --------------------------------------------------------------------
# Add any compiler flags that are uniquely required for this project

# Project-specific C flags
PROJ_CFLAGS += -DNRFX_ADC_ENABLED -DAPP_TIMER_KEEPS_RTC_ACTIVE=1 -DNRFX_PWM_ENABLED=1 -DNRFX_PWM0_ENABLED=1
PROJ_CFLAGS += -DFDS_VIRTUAL_PAGES=110 -DNRFX_TWIM_ENABLED=1 -DNRFX_TWIM0_ENABLED=1
PROJ_CFLAGS += -DNRF_SDH_BLE_GATT_MAX_MTU_SIZE=460 -DNRF_SDH_BLE_GAP_DATA_LENGTH=251

# Project-specific C++ flags
PROJ_CXXFLAGS += -DNRFX_ADC_ENABLED -DAPP_TIMER_KEEPS_RTC_ACTIVE=1 -DNRFX_PWM_ENABLED=1 -DNRFX_PWM0_ENABLED=1
PROJ_CXXFLAGS += -DFDS_VIRTUAL_PAGES=110 -DNRFX_TWIM_ENABLED=1 -DNRFX_TWIM0_ENABLED=1
PROJ_CXXFLAGS += -DNRF_SDH_BLE_GATT_MAX_MTU_SIZE=460 -DNRF_SDH_BLE_GAP_DATA_LENGTH=251

# Project-specific assembler flags
PROJ_ASMFLAGS +=

# Project-specific linker flags
PROJ_LDFLAGS +=



#### Project makefile footer ########################################################
# Must be included at the bottom of project makefile! Do not edit.

# Include common project build makefile.
include $(PROJ_DIR)/build/Makefile.common
#####################################################################################
