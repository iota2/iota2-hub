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
#include <i2_led.h>
#include <i2_stm32f4xx_hal_gpio.h>

/* Private variables ---------------------------------------------------------*/
static i2_gpio_inst_t led[MAX_NUM_LEDS] = {
  { "LED1", GPIOI, GPIO_PIN_8 },
  { "LED2", GPIOI, GPIO_PIN_9 },
};

/* Public functions ----------------------------------------------------------*/
void i2_led_init(void)
{
  int32_t i;

  i2_gpio_init();

  for ( i = 0; i < MAX_NUM_LEDS; i++ ) {
    i2_gpio_config_out(&led[i], false);
  }
}

void i2_led_on(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_config_out(&led[led_idx], true);
  }
}

void i2_led_off(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_config_out(&led[led_idx], false);
  }
}

void i2_led_toggle(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_toggle(&led[led_idx]);
  }
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
