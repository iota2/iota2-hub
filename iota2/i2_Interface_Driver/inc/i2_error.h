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
#include <stdbool.h>

/* Exported define -----------------------------------------------------------*/
#define I2_SUCCESS                  ( 0 )
#define I2_FAILURE                  ( 1 )
#define I2_TIMEOUT                  ( 2 )
#define I2_BUSY                     ( 3 )
#define I2_INVALID_PARAM            ( 4 )
#define I2_NOT_AVAILABLE            ( 5 )
#define I2_NOT_IMPLEMENTED          ( 6 )
#define I2_NOT_SUPPORTED            ( 7 )
#define I2_READY_TO_SEND            ( 8 )
#define I2_DATA_IS_CURRENT          ( 9 )
#define I2_SSL_CON_CLOSED           ( 10 )

/* Exported constants --------------------------------------------------------*/
typedef int32_t i2_error;

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
