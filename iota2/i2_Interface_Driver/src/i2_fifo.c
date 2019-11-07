/**
 * @author      iota square [i2]
 * <pre>
 * ██╗ ██████╗ ████████╗ █████╗ ██████╗
 * ██║██╔═══██╗╚══██╔══╝██╔══██╗╚════██╗
 * ██║██║   ██║   ██║   ███████║ █████╔╝
 * ██║██║   ██║   ██║   ██╔══██║██╔═══╝
 * ██║╚██████╔╝   ██║   ██║  ██║███████╗
 * ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝
 * </pre>
 *
 * @date        16-09-2019
 * @file        i2_fifo.c
 * @brief       FIFO buffer implementation.
 *
 * @copyright   GNU GPU v3
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
/**
 * @brief   Initializes FIFO context.
 * @details Initialize all components for FIFO contexts.
 *
 * @param[in] fifo        FIFO context to initialize @ref i2_fifo_t.
 * @param[in] buf         Buffer to allocation to FIFO context.
 * @param[in] fifo_size   Size of FIFO.
 * @return  None.
 */
void i2_fifo_init(i2_fifo_t *fifo, uint8_t *buf, int32_t fifo_size)
{
  i2_fifo_reset(fifo);
  fifo->fifo_size = fifo_size;
  fifo->buf = buf;
  memset(buf, 0, fifo_size);
}

/**
 * @brief   Reset FIFO context.
 * @details Reset read and write indexes of FIFO.
 *
 * @param[in] fifo    FIFO context to reset @ref i2_fifo_t.
 * @return  None.
 */
void i2_fifo_reset(i2_fifo_t *fifo)
{
  fifo->wr_index = fifo->rd_index = 0;
}

/**
 * @brief   Get size of FIFO context.
 * @details This function will return FIFO size @ref i2_fifo_t.
 *
 * @param[in] fifo    FIFO context @ref i2_fifo_t.
 * @return  Size of FIFO context.
 */
int32_t i2_fifo_size(i2_fifo_t *fifo)
{
  return fifo->fifo_size;
}

/**
 * @brief   Get count of FIFO buffer.
 * @details Returns the difference between write and read buffer.
 *
 * @param[in] fifo      FIFO context @ref i2_fifo_t.
 * @param[in] in_isr    Is this function is called from an ISR or not.
 * @return  The difference between size of write and read buffers.
 */
int32_t i2_fifo_count(i2_fifo_t *fifo, bool in_isr)
{
  int32_t count;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (!in_isr) {
    vPortEnterCritical();
  }
#else
  UNUSED_PARAMETER(in_isr);
#endif

  count = fifo->wr_index - fifo->rd_index;
  if (count < 0) {
    count += fifo->fifo_size;
  }

  if (count < fifo->fifo_size) {
    fifo->wr_index %= fifo->fifo_size;
    fifo->rd_index %= fifo->fifo_size;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (!in_isr) {
    vPortExitCritical();
  }
#endif

  return count;
}

/**
 * @brief   Write data to FIFO buffer.
 * @details Write data to FIFO.
 *
 * @param[in] fifo      FIFO context to write @ref i2_fifo_t.
 * @param[in] data      Data to write on FIFO.
 * @param[in] in_isr    Is this function is called from an ISR or not.
 * @return  If Success returns FIFO count else ( -1 ) when error occurred.
 */
int32_t i2_fifo_write(i2_fifo_t *fifo, uint8_t data, bool in_isr)
{
  int32_t retval = 0;

  if (i2_fifo_count(fifo, in_isr) < fifo->fifo_size) {
    fifo->buf[fifo->wr_index % fifo->fifo_size] = data;
    fifo->wr_index++;
    retval = i2_fifo_count(fifo, in_isr);
  } else {
    /* FIFO Full */
    retval = -1;
  }

  return retval;
}

/**
 * @brief   Read data from FIFO buffer.
 * @details Read data from FIFO.
 *
 * @param[in] fifo      FIFO context to read @ref i2_fifo_t.
 * @param[in] *data     Data read from FIFO.
 * @param[in] in_isr    Is this function is called from an ISR or not.
 * @return  If Success returns FIFO count else ( -1 ) when error occurred.
 */
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

/**
 * @brief   Read a buffer from FIFO.
 * @details Reads specified bytes count from FIFO and & copy it to destination.
 *
 * @param[in]     fifo      FIFO context to read @ref i2_fifo_t.
 * @param[in]     dst       Destination to copy buffer.
 * @param[in,out] size      Size to read form buffer.
 *                          Updated size of bytes read from FIFO is returned.
 * @param[in]     in_isr    Is this function is called from an ISR or not.
 * @return  None.
 */
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

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
