#
# @author       iota square [i2]
# <pre>
# ██╗ ██████╗ ████████╗ █████╗ ██████╗
# ██║██╔═══██╗╚══██╔══╝██╔══██╗╚════██╗
# ██║██║   ██║   ██║   ███████║ █████╔╝
# ██║██║   ██║   ██║   ██╔══██║██╔═══╝
# ██║╚██████╔╝   ██║   ██║  ██║███████╗
# ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝
# </pre>
#
# @date         16-09-2019
# @file         drivers/STM32F4xx_HAL_Driver/makefile
# @brief       	Makefile for STM32F4xx HAL drivers.
#
# @copyright    GNU GPU v3
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

ifeq ($(VERBOSE_LEVEL),2)
# All make debug messages will be printed
else
.SILENT:
endif

LIB_OUT = lib_stm32f4xx_hal.a

SRCS := ./Src/stm32f4xx_hal.c
SRCS += ./Src/stm32f4xx_hal_adc.c
SRCS += ./Src/stm32f4xx_hal_cortex.c
SRCS += ./Src/stm32f4xx_hal_crc.c
SRCS += ./Src/stm32f4xx_hal_cryp.c
SRCS += ./Src/stm32f4xx_hal_dma.c
SRCS += ./Src/stm32f4xx_hal_eth.c
SRCS += ./Src/stm32f4xx_hal_exti.c
SRCS += ./Src/stm32f4xx_hal_flash.c
SRCS += ./Src/stm32f4xx_hal_flash_ex.c
SRCS += ./Src/stm32f4xx_hal_gpio.c
SRCS += ./Src/stm32f4xx_hal_hash.c
SRCS += ./Src/stm32f4xx_hal_i2c.c
SRCS += ./Src/stm32f4xx_hal_iwdg.c
SRCS += ./Src/stm32f4xx_hal_pwr.c
SRCS += ./Src/stm32f4xx_hal_pwr_ex.c
SRCS += ./Src/stm32f4xx_hal_rcc.c
SRCS += ./Src/stm32f4xx_hal_rcc_ex.c
SRCS += ./Src/stm32f4xx_hal_rng.c
SRCS += ./Src/stm32f4xx_hal_rtc.c
SRCS += ./Src/stm32f4xx_hal_rtc_ex.c
SRCS += ./Src/stm32f4xx_hal_spi.c
SRCS += ./Src/stm32f4xx_hal_tim.c
SRCS += ./Src/stm32f4xx_hal_uart.c
SRCS += ./Src/stm32f4xx_hal_usart.c
SRCS += ./Src/stm32f4xx_hal_wwdg.c

LIB_OBJS = $(sort $(patsubst %.c,%.o,$(SRCS)))

GCOV_GCNO = $(sort $(patsubst %.c,%.gcno,$(SRCS)))
GCOV_GCOV = $(sort $(patsubst %.c,%.gcov,$(SRCS)))

.PHONY: all
all: $(LIB_OUT)
	@echo Build library $(LIB_OUT)

$(LIB_OUT): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

.PHONY: clean
clean:
	-rm -f $(LIB_OBJS) $(LIB_OUT) $(GCOV_GCNO) $(GCOV_GCOV)

# *********************** (C) COPYRIGHT iota2 ***[i2]******END OF FILE**********
