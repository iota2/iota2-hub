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

/* Public define -------------------------------------------------------------*/
typedef struct {
  int32_t wr_index;
  int32_t rd_index;
  int32_t fifo_size;
  int32_t count;
  uint8_t *buf;
} i2_fifo_t ;

/* Public functions --------------------------------------------------------- */
void i2_fifo_init(i2_fifo_t *fifo, uint8_t *buf, int32_t fifo_size);
void i2_fifo_reset(i2_fifo_t *fifo);
int32_t i2_fifo_size(i2_fifo_t *fifo);
int32_t i2_fifo_count(i2_fifo_t *fifo, bool in_isr);
int32_t i2_fifo_write(i2_fifo_t *fifo, uint8_t data, bool in_isr);
int32_t i2_fifo_read(i2_fifo_t *fifo, uint8_t *data, bool in_isr);
void i2_fifo_copy(i2_fifo_t *fifo, uint8_t *dst, int32_t *size, bool in_isr);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
