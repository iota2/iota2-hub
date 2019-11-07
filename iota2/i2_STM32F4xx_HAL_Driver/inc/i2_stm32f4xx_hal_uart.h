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
 * @file        i2_stm32f4xx_hal_uart.h
 * @brief       UART set-up and control.
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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include "i2_error.h"
#include "i2_fifo.h"

/* Public MACROS ------------------------------------------------------------ */
/** @brief Size of UART FIFO */
#define I2_UART_FIFO_SIZE       ( 1024 )
/** @brief Half of the UART FIFO */
#define I2_UART_FIFO_HALF_SIZE  ( I2_UART_FIFO_SIZE / 2 )

/* Public definitions --------------------------------------------------------*/
/**
 * @defgroup i2_uart_inst_t UART peripheral instance.
 * Defines the operational instance for UART peripheral.
 *
 * @{
 *
 */
/** @brief UART instance */
typedef struct {
  const char *inst_name;          /**< UART instance name */
  const char *ctx_name;           /**< UART context name  */
} i2_uart_inst_t;
/** @} */ /* i2_uart_inst_t */

/* Public functions --------------------------------------------------------- */
i2_error i2_uart_init(i2_uart_inst_t *inst);
i2_error i2_uart_reset(i2_uart_inst_t *inst);
i2_uart_inst_t* i2_uart_inst_get(char *inst_name);
i2_error i2_uart_tx(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                    int32_t *num_bytes, uint32_t timeout);
i2_error i2_uart_tx_polling(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                            int32_t *num_bytes, uint32_t timeout);
i2_error i2_uart_rx_buffering_start(i2_uart_inst_t *inst);
i2_error i2_uart_rx_buffering_stop(i2_uart_inst_t *inst);
i2_error i2_uart_rx(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                    int32_t *num_bytes, uint32_t timeout);
i2_error i2_uart_rx_polling(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                            int32_t *num_bytes, uint32_t timeout);
i2_error i2_uart_rx_trigger_level_set(i2_uart_inst_t *inst,
                                      int32_t rx_trigger_level);
i2_error i2_uart_rx_trigger_level_get(i2_uart_inst_t *inst,
                                      int32_t *rx_trigger_level);
i2_error i2_uart_rx_water_mark_get( i2_uart_inst_t *inst,
                                    int32_t *rx_water_mark);
i2_error i2_uart_rx_byte_count_get(i2_uart_inst_t *inst, int32_t *count);
i2_error i2_uart_rx_fifo_size_get(i2_uart_inst_t *inst, int32_t *size);
i2_error i2_uart_baud_rate_set(i2_uart_inst_t *inst, int32_t baud_rate);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
