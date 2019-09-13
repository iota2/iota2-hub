#
# @date         07-09-2019
# @author       iota square <i2>
#  _       _        ___  
# (_)     | |      |__ \.
#  _  ___ | |_ __ _   ) |
# | |/ _ \| __/ _` | / / 
# | | (_) | || (_| |/ /_ 
# |_|\___/ \__\__,_|____|
#
# @License      GNU GPU v3
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Free Software, Hell Yeah!
#

NAME           := IOTA2_CONN

GLOBAL_DEFINES := $(NAME)
GLOBAL_DEFINES += STM32F40XX
GLOBAL_DEFINES += USE_STDPERIPH_DRIVER
export GLOBAL_DEFINES

TOOLS_ROOT         := $(CURDIR)/tools
TOOLCHAIN_PREFIX   := arm-none-eabi-
TOOLCHAIN_PATH     := $(TOOLS_ROOT)/ARM_GNU/Linux64/bin/

# Default disable make verbose level
VERBOSE_LEVEL      ?= 0

ifeq ($(VERBOSE_LEVEL),2)
GLOBAL_DEFINES += VERBOSE_LEVEL=2
else ifeq ($(VERBOSE_LEVEL),1)
GLOBAL_DEFINES += VERBOSE_LEVEL=1
else ifeq ($(VERBOSE_LEVEL),0)
GLOBAL_DEFINES += VERBOSE_LEVEL=0
.SILENT:
endif

export CC      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
export CCDEP   := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
export LD      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
export AR      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ar
export AS      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
export OBJCOPY := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy
export OBJDUMP := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objdump
export GDB     := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gdb
export SIZE    := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)size

OUTPUT_ROOT = build/
OUTPUT      = $(OUTPUT_ROOT)/IOTA2_CONN/binary
BIN         = $(OUTPUT)/IOTA2_CONN.bin

GIT_HASH   := $(shell git describe --dirty --always --abbrev=0)

CPU         = -mthumb -mcpu=cortex-m4 -mfloat-abi=soft
STM32_OPT   = -DSTM32F407xx -DSTM32F40XX -DUSE_STDPERIPH_DRIVER -DUSE_FULL_ASSERT
OTHER_OPT   = "-D__weak=__attribute__((weak))" "-D__packed=__attribute__((__packed__))" -DENABLE_ASSERT -DENABLE_DEBUG
LDSCRIPT    = ./STM32F407IGTx_FLASH.ld

DRV_DIR    := drivers/

LIBINC     := -Iapp/inc
LIBINC     += -I$(DRV_DIR)/STM32F4xx_StdPeriph_Driver/inc
LIBINC     += -Idrivers/CMSIS/Include
LIBINC     += -Idrivers/CMSIS/Device/ST/STM32F4xx/Include

LIBS       := drivers/STM32F4xx_StdPeriph_Driver/lib_stm32f4xx_stdperiph.a

INCLUDES    = $(LIBINC)
CFLAGS     += $(CPU) $(STM32_OPT) $(OTHER_OPT)
CFLAGS     += -Os -Wall -fno-common -fno-short-enums
CFLAGS     += $(INCLUDES) -Wfatal-errors -std=gnu99 -DGIT_VERSION=\"$(GIT_HASH)\"
CFLAGS     += -Werror

export CFLAGS

ASFLAGS         = $(CFLAGS) -x assembler-with-cpp
LDFLAGS         = -Wl,--gc-sections,-Map=$*.map,-cref -fno-short-enums -Wl,--no-enum-size-warning -T $(LDSCRIPT) $(CPU)
ARFLAGS         = cr
OBJCOPYFLAGS    = -Obinary
OBJDUMPFLAGS    = -S

VPATH       = app/src/:
VPATH      += drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates:
VPATH      += drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7

ASMS        = drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40xx.s
		
SRCS        = drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c \
		      app/src/main.c \
		      app/src/stm32f4xx_it.c

ASMS_TEMP  := $(notdir $(ASMS))
SRCS_TEMP  := $(notdir $(SRCS))

AOBJS      := $(patsubst %.s, $(OUTPUT_ROOT)/%.o, $(ASMS_TEMP) )
OBJS       := $(patsubst %.c, $(OUTPUT_ROOT)/%.o, $(SRCS_TEMP) )

DEPS        = $(AOBJS:.o=.d)
DEPS       += $(OBJS:.o=.d)


all: $(OUTPUT) $(BIN)

flash: $(BIN)
	@echo "Starting Flashing..."
	st-flash --format binary write $< 0x8025800

erase:
	st-flash erase

test: $(AOBJS) $(OBJS)
	@echo ----- ASM -----
	@echo $(ASMS)
	@echo ----- SRC -----
	@echo $(SRCS)
	@echo ---- AOBJ -----
	@echo $(AOBJS)
	@echo ----- OBJ -----
	@echo $(OBJS)
	@echo ----- DEP -----
	@echo $(DEPS)

$(BIN): $(OUTPUT)/IOTA2_CONN.out
	$(OBJCOPY) $(OBJCOPYFLAGS) $(OUTPUT)/IOTA2_CONN.out $(BIN)
	$(OBJCOPY) -O ihex $(OUTPUT)/IOTA2_CONN.out $(OUTPUT)/IOTA2_CONN.hex
	$(OBJDUMP) $(OBJDUMPFLAGS) $(OUTPUT)/IOTA2_CONN.out > $(OUTPUT)/IOTA2_CONN.list
	$(SIZE) $(OUTPUT)/IOTA2_CONN.out
	@echo HEAD GIT HASH : $(GIT_HASH)
	@echo Make finished

$(OUTPUT)/IOTA2_CONN.out: $(LIBS) $(AOBJS) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(AOBJS) $(OBJS) $(LIBS)

$(LIBS): libs

libs:
	@$(MAKE) -C $(DRV_DIR)

libclean: clean
	@$(MAKE) -C $(DRV_DIR) clean

clean:
	@echo Removing executables...
	rm -fr $(OUTPUT_ROOT)
	@$(MAKE) -C $(DRV_DIR) clean
	
$(OUTPUT):
	mkdir -p $(OUTPUT)

-include $(DEPS)
	
$(OUTPUT_ROOT)/%.o: %.c
ifeq ($(VERBOSE_LEVEL),1)
	@echo cc $<
endif
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(@:.o=.d)

$(OUTPUT_ROOT)/%.o: %.s
ifeq ($(VERBOSE_LEVEL),1)
	@echo as $<
endif
	@$(AS) $(ASFLAGS) -c -o $@ $<

gcc_path:
	@echo "Set paths for toolchain:"
	@echo "\tCC:      $(CC)"
	@echo "\tCCDEP:   $(CCDEP)"
	@echo "\tLD:      $(LD)"
	@echo "\tAR:      $(AR)"
	@echo "\tAS:      $(AS)"
	@echo "\tOBJCOPY: $(OBJCOPY)"
	@echo "\tOBJDUMP: $(OBJDUMP)"
	@echo "\tGDB:     $(GDB)"
	@echo "\tSIZE:    $(SIZE)"

help:
	@echo "Make Options:"
	@echo "[all]           make complete project"
	@echo "[libs]          make libraries again"
	@echo "[clean]         Clean complete project"
	@echo "[libclean]      Clean all libraries"
	@echo "[test]          Test complete project by test making all targets"
	@echo "[gcc_path]      Display all configured GCC paths"	
	@echo "[flash]         Flash build to target"
	@echo "[erase]         Erase target"
	@echo "\nMake Configurations:"
	@echo "[VERBOSE_LEVEL]"
	@echo "   Define the make verbose level to print debug messages"
	@echo "     0 : No make message will be printed"
	@echo "     1 : Only additional make messages will be printed"
	@echo "     2 : All debug messages of make will be printed"

# *********************** (C) COPYRIGHT iota2 ************END OF FILE**********

