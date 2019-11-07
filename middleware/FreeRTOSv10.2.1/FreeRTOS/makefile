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
# @date         14-09-2019
# @file         middleware/FreeRTOSv10.2.1/FreeRTOS/makefile
# @brief       	Makefile for FreeRTOS.
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

LIB_OUT = lib_freertos_v10_2_1.a

SRCS :=	./Source/portable/MemMang/heap_4.c
SRCS +=	./Source/portable/GCC/ARM_CM4F/port.c
SRCS += ./Source/event_groups.c
SRCS += ./Source/list.c
SRCS += ./Source/queue.c
SRCS += ./Source/stream_buffer.c
SRCS += ./Source/tasks.c
SRCS += ./Source/timers.c

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
