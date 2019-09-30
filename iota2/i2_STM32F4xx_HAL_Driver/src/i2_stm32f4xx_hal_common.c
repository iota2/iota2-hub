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
#include <i2_stm32f4xx_hal_common.h>

/* Private define ------------------------------------------------------------*/
#define MAX_NUM_ERROR                               4

/* Private variables ---------------------------------------------------------*/
typedef struct {
  HAL_StatusTypeDef status;
  i2_error error;
} i2_error_t;

static i2_error_t hal_error[MAX_NUM_ERROR] = {
  { HAL_OK,      I2_SUCCESS},
  { HAL_ERROR,   I2_FAILURE},
  { HAL_BUSY,    I2_BUSY},
  { HAL_TIMEOUT, I2_TIMEOUT},
};

/* Public functions ---------------------------------------------------------*/
i2_error i2_get_hal_error(HAL_StatusTypeDef error)
{
  int32_t i;

  for (i = 0; i < MAX_NUM_ERROR; i++) {
    if (hal_error[i].status == error) {
      return hal_error[i].error;
    }
  }

  return I2_FAILURE;
}

void i2_delay_tick(uint32_t tick)
{
  HAL_Delay(tick);
  return;
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
