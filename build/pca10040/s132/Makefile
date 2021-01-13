# Platform-specific makefile.
#
# This makefile builds firmware for Nordic 52832 hardware platform.
#
# Contained here are hardware platform-specific options, includes,
# flags and targets. This makefile is not meant to be modified
# often after initial project configuration - project specific
# options should be settable through project's main makefile.
#
# This makefile utilizes a makefile provided with Nordic SDK to verify
# and build application .hex file.
#
#
# aconno d.o.o.
# Author: Hrvoje Brezak (hrvoje@aconno.de)
#


# --------------------------------------------------------------------
# Platform specific details
# --------------------------------------------------------------------
TARGET := nrf52832_xxaa
SOFTDEVICE := s132


# --------------------------------------------------------------------
# Platform specific filenames
# --------------------------------------------------------------------
SOFTDEVICE_HEX := s132_nrf52_6.1.0_softdevice.hex
BOOTLOADER_SETTINGS_HEX := bootloader_settings_nrf52832_s132.hex
LINKER_SCRIPT_FILE := ble_app_hrs_freertos_gcc_nrf52832_s132.ld


# --------------------------------------------------------------------
# Platform specific paths
# --------------------------------------------------------------------
PLATFORM_DIR := $(PROJ_DIR)/build/pca10040/s132


# --------------------------------------------------------------------
# Platform specific includes
# --------------------------------------------------------------------
# Include makefiles shared between all hardware platforms
include $(TEMPLATE_PATH)/Makefile.common


# --------------------------------------------------------------------
# Platform specific linker scripts
# --------------------------------------------------------------------
# Specify linker script for nRF52832 chip.
$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := $(PLATFORM_DIR)/$(LINKER_SCRIPT_FILE)

# --------------------------------------------------------------------
# Platform specific files
# --------------------------------------------------------------------
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis.c \
  $(SDK_ROOT)/external/freertos/portable/GCC/nrf52/port.c

INC_FOLDERS += \
  $(PROJ_DIR)/build/pca10040/s132 \
  $(SOFTDEVICE_DIR)/s132/headers/nrf52 \
  $(SOFTDEVICE_DIR)/s132/headers


# --------------------------------------------------------------------
# Platform specific compilation flags
# --------------------------------------------------------------------

# Platform specific flags for linker, ass, c and c++
# tells which cpu is used
PLAT_COMMON_FLAGS += -mcpu=cortex-m4
# tells which fpu is used
PLAT_COMMON_FLAGS += -mfpu=fpv4-sp-d16

# Platform spcific flags for ass, c and c++
PLAT_A_C_FLAGS += $(PLAT_COMMON_FLAGS)
PLAT_A_C_FLAGS += -DBOARD_PCA10040
PLAT_A_C_FLAGS += -DNRF52
PLAT_A_C_FLAGS += -DNRF52832_XXAA
PLAT_A_C_FLAGS += -DS132
PLAT_A_C_FLAGS += -D__HEAP_SIZE=1024
PLAT_A_C_FLAGS += -D__STACK_SIZE=2048
PLAT_A_C_FLAGS += -DNRF52_PAN_74
PLAT_A_C_FLAGS += -DNRF_SD_BLE_API_VERSION=6
PLAT_A_C_FLAGS += -DSOFTDEVICE_PRESENT
PLAT_A_C_FLAGS += -DNRF_DFU_SVCI_ENABLED
PLAT_A_C_FLAGS += -DNRF_DFU_TRANSPORT_BLE=1
PLAT_A_C_FLAGS += -DBL_SETTINGS_ACCESS_ONLY

# Platform-specific C flags
CFLAGS += $(PLAT_A_C_FLAGS)

# Platform specific c++ flags
CXXFLAGS += $(PLAT_A_C_FLAGS)

# Platform-specific assembler flags
ASMFLAGS += $(PLAT_A_C_FLAGS)

# Platform-specific linker flags
LDFLAGS += $(PLAT_COMMON_FLAGS)
LDFLAGS += -T$(LINKER_SCRIPT)


# --------------------------------------------------------------------
# Main platform-specific targets
# --------------------------------------------------------------------
.PHONY: default upload merge_all merge_soft flash_all generate_hex

# Generate nrf52832_xxaa target using SDK Makefile function.
# This abstracts away details of how SDK compiles software and
# protects project from changes in SDK structure.
default: $(call define_target, $(TARGET))
	@echo nrf52832 compilation finished!

upload: .generate_bootloader_settings merge_all flash_all
	@echo nrf52832 upload finished!

generate_hex: .generate_bootloader_settings merge_all
	@echo Generated $(PROJECT_NAME)_$(TARGET).hex file inside $(OUTPUT_DIRECTORY)/ directory!
	@echo To program aconno beacon with this firmware, use acnPROG drag and drop procedure with this file.
	@echo

pack_OTA_update:
ifeq ($(ENABLE_OTA_UPDATES), YES)
	@echo
	@echo Generating OTA update package
	@echo =============================
	@$(PROJ_DIR)/build/tools/OTA_package_generator.sh $(TARGET)
	@echo
else
	@echo Error - enable OTA updates to generate OTA update package!
endif

# --------------------------------------------------------------------
# Helper platform-specific targets
# --------------------------------------------------------------------
# Merge four hexes (Device Firmware Update module is active)
merge_all: 
	@echo
ifeq ($(ENABLE_OTA_UPDATES), YES)
	@echo Merge hex files
	@echo ===============
	@echo "Device Firmware Update (DFU) module active, performing two step merge..."
	@echo
	@echo Merging hex I
	@echo =============
	@echo Input files:
	@echo "    DFU bootloader: dfu_bootloader_$(TARGET)_$(SOFTDEVICE).hex"
	@echo "    Application hex: $(OUTPUT_DIRECTORY)/$(TARGET).hex"
	@mergehex -m $(PLATFORM_DIR)/dfu_bootloader_$(TARGET)_$(SOFTDEVICE).hex $(OUTPUT_DIRECTORY)/$(TARGET).hex -o $(OUTPUT_DIRECTORY)/mergedBLandApp.hex
	@echo
	@echo Merging hex II
	@echo ==============
	@echo Input files:
	@echo "    Bootloader settings page: $(OUTPUT_DIRECTORY)/$(BOOTLOADER_SETTINGS_HEX)"
	@echo "    SoftDevice: $(SOFTDEVICE_DIR)/$(SOFTDEVICE)/hex/$(SOFTDEVICE_HEX)"
	@echo "    Previously merged hex: $(OUTPUT_DIRECTORY)/mergedBLandApp.hex"
	@mergehex -m $(OUTPUT_DIRECTORY)/$(BOOTLOADER_SETTINGS_HEX) $(SOFTDEVICE_DIR)/$(SOFTDEVICE)/hex/$(SOFTDEVICE_HEX) $(OUTPUT_DIRECTORY)/mergedBLandApp.hex -o $(OUTPUT_DIRECTORY)/$(PROJECT_NAME)_$(TARGET).hex
	@echo
else 
	make merge_soft 
endif

# Merge the program and softdevice only (Device Firmware Update module is not active)
merge_soft:
	@echo Merge hex files
	@echo ===============
	@echo "Device Firmware Update (DFU) module not active, performing one step merge..."
	@echo
	@echo Merging hex I
	@echo ==============
	@echo Input files:
	@echo "    Application hex: $(OUTPUT_DIRECTORY)/$(TARGET).hex"
	@echo "    SoftDevice: $(SOFTDEVICE_DIR)/$(SOFTDEVICE)/hex/$(SOFTDEVICE_HEX)"
	@mergehex -m $(SOFTDEVICE_DIR)/$(SOFTDEVICE)/hex/$(SOFTDEVICE_HEX) $(OUTPUT_DIRECTORY)/$(TARGET).hex -o $(OUTPUT_DIRECTORY)/$(PROJECT_NAME)_$(TARGET).hex
	
# Flash the program and softdevice
flash_all:
	@echo Flashing
	@echo ========
	@echo Flashing file $(OUTPUT_DIRECTORY)/$(PROJECT_NAME)_$(TARGET).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(PROJECT_NAME)_$(TARGET).hex --sectorerase
	nrfjprog -f nrf52 --reset
	@echo

.generate_bootloader_settings:
ifeq ($(ENABLE_OTA_UPDATES), YES)
	@echo 
	@echo Generating bootloader settings file
	@echo ===================================
	@nrfutil settings generate --family NRF52 --application $(OUTPUT_DIRECTORY)/$(TARGET).hex --application-version 0 --app-boot-validation VALIDATE_GENERATED_CRC --bootloader-version 0 --bl-settings-version 2 $(OUTPUT_DIRECTORY)/$(BOOTLOADER_SETTINGS_HEX)
endif