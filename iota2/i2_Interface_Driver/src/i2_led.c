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
 * @file        i2_led.c
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

/* Includes ------------------------------------------------------------------*/
#include <i2_led.h>
#include <i2_stm32f4xx_hal_gpio.h>

/* Private variables ---------------------------------------------------------*/
/**
 * @defgroup I2_LED LED pin specification.
 * LED mapping to GPIOs.
 *
 * @{
 */
/** @brief LED Pins configuration */
static i2_gpio_inst_t led[MAX_NUM_LEDS] = {
  { "LED1", GPIOI, GPIO_PIN_8 },              /**< LED1 pin configuration */
  { "LED2", GPIOI, GPIO_PIN_9 },              /**< LED2 pin configuration */
};
/** @} */ /* I2_LED */

/* Public functions ----------------------------------------------------------*/
/**
 * @brief   Initializes LED components.
 * @details Initialize all LED defined under I2_LED specifications.
 *
 * @return  None.
 */
void i2_led_init(void)
{
  int32_t i;

  i2_gpio_init();

  for ( i = 0; i < MAX_NUM_LEDS; i++ ) {
    i2_gpio_config_out(&led[i], I2_LOW);
  }
}

/**
 * @brief   Turns LED on.
 * @details Turns on specific LED.
 *
 * @param[in] led_idx     Index of LED to turn on @ref i2_led_t.
 * @return  None.
 */
void i2_led_on(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_config_out(&led[led_idx], I2_HIGH);
  }
}

/**
 * @brief   Turns LED off.
 * @details Turns off specific LED.
 *
 * @param[in] led_idx     Index of LED to turn off @ref i2_led_t.
 * @return  None.
 */
void i2_led_off(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_config_out(&led[led_idx], I2_LOW);
  }
}

/**
 * @brief   Toggles LED state.
 * @details Toggles current state of LED.
 *
 * @param[in] led_idx    Index of LED to toggle @ref i2_led_t.
 * @return  None.
 */
void i2_led_toggle(i2_led_t led_idx)
{
  if ( MAX_NUM_LEDS > led_idx ) {
    i2_gpio_toggle(&led[led_idx]);
  }
}

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
