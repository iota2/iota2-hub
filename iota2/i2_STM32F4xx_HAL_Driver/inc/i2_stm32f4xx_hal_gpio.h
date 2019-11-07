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
 * @file        i2_stm32f4xx_hal_gpio.h
 * @brief       GPIO set-up and control.
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
#include <i2_stm32f4xx_hal_common.h>

/* Public definitions --------------------------------------------------------*/
/**
 * @defgroup i2_gpio_inst_t GPIO interface instance.
 * Instance defining all parameters for GPIO interfacing.
 *
 * @{
 */
/** @brief GPIO instance */
typedef struct {
  const char    *name;        /**< Pin identification name    */
  GPIO_TypeDef  *gpio_port;   /**< GPIO port to configure     */
  uint32_t      gpio;         /**< GPIO pin to configure      */
  uint32_t      speed;        /**< GPIO pin speed             */
  uint32_t      pull;         /**< Push / Pull configuration  */
  uint32_t      mode;         /**< GPIO operation mode        */
  uint32_t      alt;          /**< Alternate function mapping */
  bool          active;       /**< Pin activation check       */
} i2_gpio_inst_t;             /**< GPIO instance              */
/** @} */ /* i2_gpio_inst_t */

/* Public functions --------------------------------------------------------- */
void i2_gpio_init(void);
i2_gpio_inst_t* i2_gpio_ctx_get(char *name);
bool i2_gpio_is_valid(i2_gpio_inst_t *inst);
void i2_gpio_config(i2_gpio_inst_t *inst, uint32_t mode, uint32_t pull);
void i2_gpio_release(i2_gpio_inst_t *inst);
void i2_gpio_config_out(i2_gpio_inst_t *inst, bool val);
void i2_gpio_config_in(i2_gpio_inst_t *inst, uint32_t pull);
void i2_gpio_config_alt(i2_gpio_inst_t *inst, uint32_t mode,
                        uint32_t pull, uint32_t alt);
bool i2_gpio_get(i2_gpio_inst_t *inst);
void i2_gpio_set(i2_gpio_inst_t *inst, bool val);
void i2_gpio_toggle(i2_gpio_inst_t *inst);
i2_error i2_gpio_config_interrupt(i2_gpio_inst_t *inst, uint32_t mode,
                                  uint32_t pull,
                                  void (*cb)(void *arg), void *arg);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
