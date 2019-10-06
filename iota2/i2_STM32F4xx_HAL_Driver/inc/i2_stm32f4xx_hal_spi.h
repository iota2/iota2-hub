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
#include <i2_error.h>

#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_spi.h"

#include "i2_stm32f4xx_hal_common.h"
#include "i2_stm32f4xx_hal_gpio.h"

/* Public definitions --------------------------------------------------------*/

/* SPI Operation Modes
 *    Mode    CPOL    CPHA
 *    0       0       0
 *    1       0       1
 *    2       1       0
 *    3       1       1
 */
typedef enum {
  I2_SPI_MODE_0 = 0,
  I2_SPI_MODE_1,
  I2_SPI_MODE_2,
  I2_SPI_MODE_3,
} i2_spi_mode_t;

/* SPI data width */
typedef enum {
  I2_SPI_DATA_WIDTH_8BIT = 0,
  I2_SPI_DATA_WIDTH_16BIT,
} i2_spi_data_width_t;

/* SPI clock speed */
typedef enum {
  I2_SPI_CLK_40_MHZ = 0,
  I2_SPI_CLK_20_MHZ,
  I2_SPI_CLK_10_MHZ,
  I2_SPI_CLK_5_MHZ,
  I2_SPI_CLK_2500_KHZ,
  I2_SPI_CLK_1250_KHZ,
  I2_SPI_CLK_625_KHZ,
  I2_SPI_CLK_313_KHZ,
} i2_spi_clock_speed_t;

/* Which bit to send first MSBit or LSBit */
typedef enum {
  I2_SPI_MSBIT_FIRST = 0,
  I2_SPI_LSBIT_FIRST,
} i2_spi_first_bit_t;

/* This defines the spi device instance. Note the chip select gpio */
typedef struct {
  const char      *inst_name;
  const char      *ctx_name;
  i2_gpio_inst_t  CS;
} i2_spi_inst_t;

/* Public functions --------------------------------------------------------- */
i2_error i2_spi_init(i2_spi_inst_t *inst);

i2_spi_inst_t* spi_inst_get(char *inst_name);

i2_error i2_spi_txrx(i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                     i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                     i2_spi_first_bit_t first_bit,
                     uint8_t *txbuf, uint8_t *rxbuf,
                     int32_t size, uint32_t timeout);

i2_error i2_spi_txrx_raw( i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                          i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                          i2_spi_first_bit_t first_bit,
                          uint8_t *txbuf, uint8_t *rxbuf,
                          int32_t size, uint32_t timeout);

i2_error i2_spi_tx( i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                    i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                    i2_spi_first_bit_t first_bit,
                    uint8_t *txbuf, int32_t size, uint32_t timeout);

i2_error i2_spi_tx_raw( i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                        i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                        i2_spi_first_bit_t first_bit,
                        uint8_t *txbuf, int32_t size, uint32_t timeout);

i2_error i2_spi_rx( i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                    i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                    i2_spi_first_bit_t first_bit,
                    uint8_t *rxbuf, int32_t size, uint32_t timeout);

i2_error i2_spi_rx_raw( i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                        i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                        i2_spi_first_bit_t first_bit,
                        uint8_t *rxbuf, int32_t size, uint32_t timeout);

i2_error i2_spi_cs_assert(i2_spi_inst_t *inst, uint32_t timeout);
i2_error i2_spi_cs_deassert(i2_spi_inst_t *inst);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/

