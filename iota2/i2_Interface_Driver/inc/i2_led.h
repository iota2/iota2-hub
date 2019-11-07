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
 * @file        i2_led.h
 * @brief       LED interface.
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
#include <stdint.h>
#include <stdbool.h>

/* Public define -------------------------------------------------------------*/
/**
 * @defgroup i2_led_t LED used in system.
 * Defines the LED's to use.
 *
 * @{
 */
/** @brief Defined LED's */
typedef enum {
  LED1 = 0,               /**< System LED 1         */
  LED2,                   /**< System LED 2         */
  MAX_NUM_LEDS,           /**< Maximum System LED's */
} i2_led_t;
/** @} */ /* i2_led_t */

/* Public functions --------------------------------------------------------- */
void i2_led_init(void);
void i2_led_on(i2_led_t led_idx);
void i2_led_off(i2_led_t led_idx);
void i2_led_toggle(i2_led_t led_idx);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
