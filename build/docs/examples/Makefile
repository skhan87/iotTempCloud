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
PROJECT_NAME     := acnSCHMERSAL


# --------------------------------------------------------------------
# Project options
# --------------------------------------------------------------------
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

# Include project-specific headers
PROJ_INC += \
  $(PROJ_DIR)/config                          \

# Include project-specific library files
PROJ_LIB_FILES += \

# --------------------------------------------------------------------
# Project compiler flags
# --------------------------------------------------------------------
# Add any compiler flags that are uniquely required for this project

# Project-specific C flags
PROJ_CFLAGS +=

# Project-specific C++ flags
PROJ_CXXFLAGS +=

# Project-specific assembler flags
PROJ_ASMFLAGS +=

# Project-specific linker flags
PROJ_LDFLAGS +=



#### Project makefile footer ########################################################
# Must be included at the bottom of project makefile! Do not edit.

# Include common project build makefile.
include $(PROJ_DIR)/build/Makefile.common
#####################################################################################
