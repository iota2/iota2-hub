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

ifeq ($(VERBOSE_LEVEL),2)
# All make debug messages will be printed 
else
.SILENT:
endif

LIB_OUT = lib_stm32f4xx_stdperiph.a

SRCS	=	./src/misc.c \
				./src/stm32f4xx_adc.c \
				./src/stm32f4xx_can.c \
				./src/stm32f4xx_flash.c \
				./src/stm32f4xx_flash_ramfunc.c \
				./src/stm32f4xx_gpio.c \
				./src/stm32f4xx_i2c.c \
				./src/stm32f4xx_iwdg.c \
				./src/stm32f4xx_lptim.c \
				./src/stm32f4xx_pwr.c \
				./src/stm32f4xx_qspi.c \
				./src/stm32f4xx_rcc.c \
				./src/stm32f4xx_rng.c \
				./src/stm32f4xx_rtc.c \
				./src/stm32f4xx_sdio.c \
				./src/stm32f4xx_spi.c \	
				./src/stm32f4xx_syscfg.c \
				./src/stm32f4xx_tim.c \
				./src/stm32f4xx_usart.c \
				./src/stm32f4xx_wwdg.c

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

