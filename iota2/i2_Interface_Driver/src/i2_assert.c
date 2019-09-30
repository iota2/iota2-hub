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

#include <i2_stm32f4xx_hal_common.h>

#include <i2_assert.h>
#include <i2_led.h>

#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

static void assert_flash_led(void)
{
  while (1) {
    /*
     * If error, toggle both LEDs fast.
     * The i value was experimentally determined
     * based on the current 80Mhz core clock.
     */
    volatile int32_t i = 0;

    i2_led_on(LED1);
    i2_led_off(LED2);
    for (i = 0; i < 0x8ffff; i++) {
        if (i == 0x8fffe) {
          i2_led_toggle(LED1);
          i2_led_toggle(LED2);
        }
    }
  }
}

void i2_assert(int32_t good)
{
  if (!good) {
    portDISABLE_INTERRUPTS();
    assert_flash_led();
  }
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
