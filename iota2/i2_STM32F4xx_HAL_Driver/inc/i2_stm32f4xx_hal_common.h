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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <i2_error.h>
#include <i2_assert.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_conf.h>

/* Public Variables ----------------------------------------------------------*/
/* Core driver HAL mode */
typedef enum {
  POLLING_MODE = 0,
  INTERRUPT_MODE,
  DMA_MODE,
} i2_hal_mode_t;

typedef struct {
  void (*cb)(void *arg);
  void *arg;
} i2_handler_t;

/* Public defines ------------------------------------------------------------*/
/* Core driver transfer status */
#define I2_TRANSFER_WAIT                  ( 0 )
#define I2_TRANSFER_DONE                  ( 1 )
#define I2_TRANSFER_ERROR                 ( 2 )

/* Definitions For Enabling / Disabling Functionalities */
#define I2_DISABLE                        ( 0 )
#define I2_ENABLE                         ( 1 )

/* Definitions For Pin HIGH / LOW Functionalities */
#define I2_LOW                            ( 0 )
#define I2_HIGH                           ( 1 )

/* Public functions ----------------------------------------------------------*/
i2_error i2_get_hal_error(HAL_StatusTypeDef err);
void i2_delay_tick(uint32_t tick);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
