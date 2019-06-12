# Target file name (without extension).
PROJECT ?= poker2

# Directory common source files exist
TMK_DIR = tmk_core
CHIBIOS = ChibiOS
CHIBIOS_CONTRIB = ChibiOS-Contrib
CONFDIR   := .
BUILDDIR  := ./build
DEPDIR    := ./.dep

# Directory keyboard dependent files exist
TARGET_DIR = .

# project specific files

SRC += hardfault.c
SRC += matrix.c
SRC += nuvoton.c

ifdef KEYMAP
    SRC := keymaps/$(PROJECT)/keymap_$(KEYMAP).c $(SRC)
else
    SRC := keymaps/$(PROJECT)/keymap_$(PROJECT).c $(SRC)
endif

CONFIG_H = config.h

## chip/board settings
# - the next two should match the directories in
#   <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
# - For PokerII, FAMILY = NUMICRO and MCU_SERIES = NUC122
MCU_FAMILY = NUMICRO

# Linker script to use
# - it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#   or <this_dir>/ld/
# - LDSCRIPT =
#   - NUC1xxxC1xx for Poker II
#   - NUC1xxxD2xx for NUC122 devboard

# Startup code to use
#  - it should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
# - STARTUP =
#   - nuc122 for NUC122SC1/NUC122SD1

# Board: it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
# - BOARD =
#   - NUC122_NUTINY for NUC122SC1/NUC122SD1
BOARD = $(PROJECT)

# Cortex version
# Teensy LC is cortex-m0plus; Teensy 3.x are cortex-m4
MCU  = cortex-m0

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
# I.e. 6 for Teensy LC; 7 for Teensy 3.x
ARMV = 6

USE_EXCEPTIONS_STACKSIZE = 0x200
USE_PROCESS_STACKSIZE = 0x400

# Build Options
#   comment out to disable the options.
#
# Be aware that you can only enable 3 more endpoints with a NUC122
# (e.g. Mouse+Extrakeys+NKRO)
#
MOUSEKEY_ENABLE = yes	# Mouse keys
#EXTRAKEY_ENABLE = yes	# Audio control and System control
CONSOLE_ENABLE = yes	# Console for debug
#COMMAND_ENABLE = yes    # Commands for debug and configuration
NKRO_ENABLE = yes	    # USB Nkey Rollover

include boards/$(PROJECT)/tmk_options.mk
include $(TMK_DIR)/tool/chibios/common.mk
include $(TMK_DIR)/tool/chibios/chibios.mk
LDFLAGS += -Wl,--just-symbols=hardfault.symbols

build/ikbc_$(PROJECT).bin: build/$(PROJECT).bin
	srec_cat '(' $(BUILDDIR)/$(PROJECT).bin -binary -fill 0xff 0 $(SREC_CAT_OFFSET_TRAILER) -generate $(SREC_CAT_OFFSET_TRAILER) $(shell expr $(SREC_CAT_OFFSET_TRAILER) + $(SREC_CAT_TRAILER) : ".*" ) -repeat-string "$(SREC_CAT_TRAILER)" ')' -checksum-neg-l-e $(SREC_CAT_OFFSET_CHECKSUM) 4 4 -o $@.crc -binary
	kbtflash/kbtflash.py obf $@.crc $@

flash: build/ikbc_$(PROJECT).bin
	kbtflash/kbtflash.py flash -r --product-id $(USB_PRODUCT_ID) $?
