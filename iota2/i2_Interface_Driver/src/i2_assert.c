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
 * @file        i2_assert.c
 * @brief       System assert functionality.
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

#include <i2_stm32f4xx_hal_common.h>

#include <i2_assert.h>
#include <i2_led.h>

#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

/**
 * @brief    Function to call when a system assert call happens.
 * @details  Here write the operations when assert happens.
 *
 * @return   None.
 */
static void assert(void)
{
  while (1) {
    /*
     * If error, toggle both LED's fast.
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

/**
 * @brief   Assert check utility.
 * @details Call this utility when needs to check asset functionality.
 *
 * @param[in] good : parameter to check.
 * @return  None.
 */
void i2_assert(int32_t good)
{
  if (!good) {
    portDISABLE_INTERRUPTS();
    assert();
  }
}

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
