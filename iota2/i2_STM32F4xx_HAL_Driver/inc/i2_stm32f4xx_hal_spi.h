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
 * @file        i2_stm32f4xx_hal_spi.h
 * @brief       SPI set-up and control.
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
#include <i2_error.h>

#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_spi.h"

#include "i2_stm32f4xx_hal_common.h"
#include "i2_stm32f4xx_hal_gpio.h"

/* Public definitions --------------------------------------------------------*/

/**
 * @defgroup i2_spi_mode_t SPI Operation Modes.
 * Defines the available operation modes for SPI peripheral.
 *
 * @{
 *
 *  | Mode  | CPOL  | CPHA  |
 *  |:-----:|:-----:|:-----:|
 *  |   0   |   0   |   0   |
 *  |   1   |   0   |   1   |
 *  |   2   |   1   |   0   |
 *  |   3   |   1   |   1   |
 *
 */
/** @brief SPI Operational mode */
typedef enum {
  I2_SPI_MODE_0 = 0,            /**< MODE 0 : CPOL = 0 & CPHA = 0   */
  I2_SPI_MODE_1,                /**< MODE 1 : CPOL = 0 & CPHA = 1   */
  I2_SPI_MODE_2,                /**< MODE 2 : CPOL = 1 & CPHA = 0   */
  I2_SPI_MODE_3,                /**< MODE 3 : CPOL = 1 & CPHA = 1   */
} i2_spi_mode_t;
/** @} */ /* i2_spi_mode_t */

/**
 * @defgroup i2_spi_data_width_t SPI data width.
 * Defines the width of one SPI data packet.
 *
 * @{
 */
/** @brief SPI Data packet width */
typedef enum {
  I2_SPI_DATA_WIDTH_8BIT = 0,   /**< Use  8 bit data packet */
  I2_SPI_DATA_WIDTH_16BIT,      /**< Use 16 bit data packet */
} i2_spi_data_width_t;
/** @} */ /* i2_spi_data_width_t */

/**
 * @defgroup i2_spi_clock_speed_t SPI clock speed.
 * Defines the available operational clock speed for SPI peripheral.
 *
 * @{
 */
/** @brief SPI clock speed */
typedef enum {
  I2_SPI_CLK_40_MHZ = 0,    /**< SPI operational clock at   40 MHz */
  I2_SPI_CLK_20_MHZ,        /**< SPI operational clock at   20 MHz */
  I2_SPI_CLK_10_MHZ,        /**< SPI operational clock at   10 MHz */
  I2_SPI_CLK_5_MHZ,         /**< SPI operational clock at    5 MHz */
  I2_SPI_CLK_2500_KHZ,      /**< SPI operational clock at 2500 KHz */
  I2_SPI_CLK_1250_KHZ,      /**< SPI operational clock at 1250 KHz */
  I2_SPI_CLK_625_KHZ,       /**< SPI operational clock at  625 KHz */
  I2_SPI_CLK_313_KHZ,       /**< SPI operational clock at  313 KHz */
} i2_spi_clock_speed_t;
/** @} */ /* i2_spi_clock_speed_t */

/**
 * @defgroup i2_spi_first_bit_t SPI First bit to send / receive.
 * Defines Which bit to send first MSBit or LSBit.
 *
 * @{
 */
/** @brief SPI First Bit */
typedef enum {
  I2_SPI_MSBIT_FIRST = 0,   /**< Consider MSB as first bit        */
  I2_SPI_LSBIT_FIRST,       /**< COnsider LSB as first bit        */
} i2_spi_first_bit_t;
/** @} */ /* i2_spi_first_bit_t */

/**
 * @defgroup i2_spi_inst_t SPI peripheral instance.
 * Defines instance details for SPI. including used Chip Select Pin definition.
 *
 * @{
 */
/** @brief SPI instance */
typedef struct {
  const char      *inst_name;   /**< SPI instance name          */
  const char      *ctx_name;    /**< SPI context  name          */
  i2_gpio_inst_t  CS;           /**< Chip select GPIO instance  */
} i2_spi_inst_t;
/** @} */ /* i2_spi_inst_t */

/* Public functions --------------------------------------------------------- */
i2_error i2_spi_init(i2_spi_inst_t *inst);

i2_spi_inst_t* i2_spi_inst_get(char *inst_name);

i2_error i2_spi_txrx(i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
                     i2_spi_clock_speed_t clk_speed, i2_spi_mode_t spi_mode,
                     i2_spi_first_bit_t first_bit,
                     uint8_t *txbuf, uint8_t *rxbuf,
                     int32_t size, uint32_t timeout);

i2_error i2_spi_txrx_raw(i2_spi_inst_t *inst, i2_spi_data_width_t data_witdh,
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

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
