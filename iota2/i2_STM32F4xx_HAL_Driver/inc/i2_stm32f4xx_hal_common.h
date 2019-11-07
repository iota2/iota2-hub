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
 * @file        i2_stm32f4xx_hal_common.h
 * @brief       HAL common configurations and set-up.
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
#include <string.h>
#include <stdbool.h>

#include <i2_error.h>
#include <i2_assert.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_conf.h>

/* Public Variables ----------------------------------------------------------*/
/**
 * @defgroup i2_hal_mode_t HAL core operational mode.
 * Configurations for different operations modes.
 *
 * @{
 */
/** @brief HAL operational modes */
typedef enum {
  POLLING_MODE = 0,       /**< Configure HAL in polling mode    */
  INTERRUPT_MODE,         /**< Configure HAL in interrupt mode  */
  DMA_MODE,               /**< Configure HAL in DMA mode        */
} i2_hal_mode_t;          /**< HAL operations modes             */
/** @} */ /* i2_hal_mode_t */

/**
 * @defgroup i2_handler_t iota2 generic callback Handlers.
 * Context to configure callback instances.
 *
 * @{
 */
/** @brief Callback Handler */
typedef struct {
  void (*cb)(void *arg);  /**< Callback to configure          */
  void *arg;              /**< Arguments to pass in callback  */
} i2_handler_t;
/** @} */ /* i2_handler_t */

/* Public defines ------------------------------------------------------------*/
/**
 * @defgroup I2_TRANSFER_STATE HAL core driver transfer status.
 * State after core driver execution.
 *
 * @{
 */
#define I2_TRANSFER_WAIT            ( 0 ) /**< Waiting for transfer complete  */
#define I2_TRANSFER_DONE            ( 1 ) /**< Transfer complete successfully */
#define I2_TRANSFER_ERROR           ( 2 ) /**< Transfer terminated with error */
/** @} */ /* I2_TRANSFER_STATE */

/**
 * @defgroup I2_DISABLE_ENABLE iota2 Enabling options.
 * Definitions For Enabling / Disabling Functionalities.
 *
 * @{
 */
#define I2_DISABLE                  ( 0 ) /**< Defines to disable a feature   */
#define I2_ENABLE                   ( 1 ) /**< Defines to enable a feature    */
/** @} */ /* I2_DISABLE_ENABLE */

/**
 * @defgroup I2_LOW_HIGH iota2 pin control settings.
 * Definitions For Pin HIGH / LOW Functionalities.
 *
 * @{
 */
#define I2_LOW                      ( 0 ) /**< Defines to reset a pin */
#define I2_HIGH                     ( 1 ) /**< Defines to set a pin   */
/** @} */ /* I2_LOW_HIGH */

/* Public functions ----------------------------------------------------------*/
i2_error i2_get_hal_error(HAL_StatusTypeDef err);
void i2_delay_tick(uint32_t tick);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
