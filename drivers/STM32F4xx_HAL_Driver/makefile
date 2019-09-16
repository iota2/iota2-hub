#
# @author       iota square <i2>
# @date         16-09-2019
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

ifeq ($(VERBOSE_LEVEL),2)
# All make debug messages will be printed 
else
.SILENT:
endif

LIB_OUT = lib_stm32f4xx_hal.a

SRCS	=	./Src/stm32f4xx_hal.c \
				./Src/stm32f4xx_hal_adc.c \
				./Src/stm32f4xx_hal_cortex.c \
				./Src/stm32f4xx_hal_crc.c \
				./Src/stm32f4xx_hal_cryp.c \
				./Src/stm32f4xx_hal_dma.c \
				./Src/stm32f4xx_hal_eth.c \
				./Src/stm32f4xx_hal_exti.c \
				./Src/stm32f4xx_hal_flash.c \
				./Src/stm32f4xx_hal_gpio.c \
				./Src/stm32f4xx_hal_hash.c \
				./Src/stm32f4xx_hal_i2c.c \
				./Src/stm32f4xx_hal_iwdg.c \
				./Src/stm32f4xx_hal_pwr.c \
				./Src/stm32f4xx_hal_rcc.c \
				./Src/stm32f4xx_hal_rng.c \
				./Src/stm32f4xx_hal_rtc.c \
				./Src/stm32f4xx_hal_spi.c \
				./Src/stm32f4xx_hal_tim.c \
				./Src/stm32f4xx_hal_uart.c \
				./Src/stm32f4xx_hal_usart.c \
				./Src/stm32f4xx_hal_wwdg.c

LIB_OBJS = $(sort $(patsubst %.c,%.o,$(SRCS))) 

.PHONY: all
all: $(LIB_OUT)
	@echo Build library $(LIB_OUT)

$(LIB_OUT): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

.PHONY: clean
clean:
	-rm -f $(LIB_OBJS) $(LIB_OUT)

# *********************** (C) COPYRIGHT iota2 ************END OF FILE**********

