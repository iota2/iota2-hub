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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include "i2_error.h"
#include "i2_fifo.h"

/* Public MACROS ------------------------------------------------------------ */
#define I2_UART_FIFO_SIZE                     ( 1024 )

/* Public definitions --------------------------------------------------------*/
typedef struct {
  const char *inst_name;
  const char *ctx_name;
} i2_uart_inst_t;

/* Public functions --------------------------------------------------------- */
i2_error i2_uart_init(i2_uart_inst_t *inst);

/*
 * Abort current TX/RX activity and re-init the UART controller
 */
i2_error i2_uart_reset(i2_uart_inst_t *inst);

i2_uart_inst_t* i2_uart_inst_get(char *inst_name);

/*
 * The interrupt/DMA based uart_tx() can be called by users after UART init.
 * It will block until the UART TX is finished.
 */
i2_error i2_uart_tx(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout);

/*
 * uart_rx_buffering_start() needs to be called by UART device drivers
 * after uart_init or uart_lpm_exit. Then the UART controller will be
 * buffering RX data into the internal FIFO buffer associated with the
 * UART controller. The FIFO buffer size is statically decided at the
 * compile time.
 */
i2_error i2_uart_rx_buffering_start(i2_uart_inst_t *inst);

/*
 * uart_rx_buffering_stop() needs to be called by UART device drivers
 * before uart_lpm_enter or before uart_rx_polling API is used. The
 * RX receiver will be disabled and the FIFO buffer will be cleared out.
 */
i2_error i2_uart_rx_buffering_stop(i2_uart_inst_t *inst);

/*
 * The interrupt/DMA based uart_rx() can be called by users after UART RX
 * buffering starts. Let's say uart_rx() is called with the caller
 * buffer of size X bytes and time out T msec.
 *
 * If there is already *any* number of bytes in the UART driver's receive FIFO,
 * uart_rx() returns right away regardless of X or T values.
 *
 * If the UART driver's receive FIFO is empty at the time of the uart_rx() call,
 * the caller will block on UART RX semaphore until one of the following
 * conditions is met.
 *
 * a) UART driver's receive FIFO gets *any* number of bytes, followed by UART RX
 *    line going idle regardless of X or T values.
 *
 * b) UART driver's receive FIFO gets filled up to half full without the UART RX
 *    line going idle regardless of X or T values.
 *
 * c) UART driver does not get any byte in the UART driver's receive FIFO until
 *    the timeout expires, then uart_rx() returns with no data. It's indicated
 *    by the *num_bytes being zero and also by the return value.
 *
 * In a) and b) cases, if the caller's buffer size X happens to be smaller than
 * the available number of UART RX bytes in the UART driver's receive FIFO, then
 * the UART driver copies X number of bytes first and the caller must call
 * uart_rx() again to read in the remaining bytes.
 *
 * It's important to remember whenever UART RX line goes idle after receiving
 * any number of bytes, uart_rx() will return. This is done to meet UART RX
 * responsiveness requirement, as UART driver can't wait until either buffer
 * gets full or timeout happens from the performance point of view.
 *
 * Also note that if the caller does not read out data in UART receive FIFO
 * fast enough, the buffered data in the FIFO will start to be overwritten
 * by new data.
 */
i2_error i2_uart_rx(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout);

/* UART polling RX API. Disable uart_rx buffering first before reading RX bytes */
i2_error i2_uart_rx_polling(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout);

/*
 * UART polling TX API. It can be used along with interrupt/DMA based uart_tx()
 * API without any special precaution.
 */
i2_error i2_uart_tx_polling(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout);

/*
 * Set the UART RX FIFO level at which the uart_rx() call's blocking semaphore
 * will be released. Thus, the calling function will have the chance to read out
 * the RX buffer to avoid the FIFO overrun. It defaults to half of the FIFO depth.
 * The UART RX buffering needs to be disabled first to successfully call it.
 */
i2_error i2_uart_rx_trigger_level_set(i2_uart_inst_t *inst, int32_t rx_trigger_level);

/*
 * Get the current RX trigger level
 */
i2_error i2_uart_rx_trigger_level_get(i2_uart_inst_t *inst, int32_t *rx_trigger_level);

/*
 * Get the current RX water mark level. The water mark is the highest FIFO level
 * at which it has ever been. This information is used to tweak the FIFO depth
 * and the trigger level programming.
 */
i2_error i2_uart_rx_water_mark_get(i2_uart_inst_t *inst, int32_t *rx_water_mark);

/*
 * Get the current number of byte count in RX FIFO.
 * It's assumed that it's called in a task context. Do not call this in ISR.
 */
i2_error i2_uart_rx_byte_count_get(i2_uart_inst_t *inst, int32_t *count);

/*
 * Get the fixed RX FIFO size. Not to be confused with uart_rx_count_count_get.
 */
i2_error i2_uart_rx_fifo_size_get(i2_uart_inst_t *inst, int32_t *size);

/*
 * Change the UART baud rate. The UART RX buffering needs to be stopped first
 */
i2_error i2_uart_baud_rate_set(i2_uart_inst_t *inst, int32_t baud_rate);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
