/**
 * @author      iota square <i2>
 * @date        16-09-2019
 *  _       _        ___
 * (_)     | |      |__ \.
 *  _  ___ | |_ __ _   ) |
 * | |/ _ \| __/ _` | / /
 * | | (_) | || (_| |/ /_
 * |_|\___/ \__\__,_|____|
 *
 * @License     GNU GPU v3
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Free Software, Hell Yeah!
 *
 **/

/* Includes ------------------------------------------------------------------*/
#include "i2_fifo.h"

#include <string.h>
#include <i2_stm32f4xx_hal_common.h>

#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

/* Public functions --------------------------------------------------------- */
void i2_fifo_init(i2_fifo_t *fifo, uint8_t *buf, int32_t fifo_size)
{
  i2_fifo_reset(fifo);
  fifo->fifo_size = fifo_size;
  fifo->buf = buf;
  memset(buf, 0, fifo_size);
}

void i2_fifo_reset(i2_fifo_t *fifo)
{
  fifo->wr_index = fifo->rd_index = 0;
}

int32_t i2_fifo_size(i2_fifo_t *fifo)
{
  return fifo->fifo_size;
}

int32_t i2_fifo_count(i2_fifo_t *fifo, bool in_isr)
{
  int32_t count;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if ( !in_isr ) {
    vPortEnterCritical();
  }
#else
  UNUSED_PARAMETER(in_isr);
#endif

  count = fifo->wr_index - fifo->rd_index;
  if ( count < 0 ) {
    count += fifo->fifo_size;
  }

  if ( count < fifo->fifo_size ) {
    fifo->wr_index %= fifo->fifo_size;
    fifo->rd_index %= fifo->fifo_size;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if ( !in_isr ) {
    vPortExitCritical();
  }
#endif

  return count;
}

int32_t i2_fifo_write(i2_fifo_t *fifo, uint8_t data, bool in_isr)
{
  int32_t retval = 0;

  if ( i2_fifo_count(fifo, in_isr) < fifo->fifo_size ) {
    fifo->buf[fifo->wr_index % fifo->fifo_size] = data;
    fifo->wr_index++;
    retval = i2_fifo_count(fifo, in_isr);
  } else {
    /* FIFO Full */
    retval = -1;
  }

  return retval;
}

int32_t i2_fifo_read(i2_fifo_t *fifo, uint8_t *data, bool in_isr)
{
  int32_t retval = 0;

  if ( i2_fifo_count(fifo, in_isr) ) {
    *data = fifo->buf[fifo->rd_index % fifo->fifo_size];
    fifo->rd_index++;
    retval = i2_fifo_count(fifo, in_isr);
  } else {
    /* FIFO empty */
    retval = -1;
  }

  return retval;
}

void i2_fifo_copy(i2_fifo_t *fifo, uint8_t *dst, int32_t *size, bool in_isr)
{
  uint8_t data;
  int32_t counter = 0;

  /* Read until given buffer is full or fifo empty */
  while ((*size > counter) && (i2_fifo_read(fifo, &data, in_isr) >= 0)) {
    *dst++ = data;
    counter++;
  }
  *size = counter;

  return;
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
