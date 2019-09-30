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
typedef enum {
  LED1 = 0,
  LED2,
  MAX_NUM_LEDS,
} i2_led_t;

/* Public functions --------------------------------------------------------- */
void i2_led_init(void);
void i2_led_on(i2_led_t led_idx);
void i2_led_off(i2_led_t led_idx);
void i2_led_toggle(i2_led_t led_idx);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
