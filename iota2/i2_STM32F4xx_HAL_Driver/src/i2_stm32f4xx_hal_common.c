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
 * @file        i2_stm32f4xx_hal_common.c
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

/* Includes ------------------------------------------------------------------*/
#include <i2_stm32f4xx_hal_common.h>

/* Private define ------------------------------------------------------------*/
#define MAX_NUM_ERROR                 4   /**< Number of mapped HAL errors    */

/* Private variables ---------------------------------------------------------*/
/**
 * @defgroup i2_error_t iota2 and HAL errors mapping context.
 * Maps a HAL error with corresponding iota2 error code.
 *
 * @{
 */
/** @brief iota2 and HAL error mapping */
typedef struct {
  HAL_StatusTypeDef status; /**< HAL status code              */
  i2_error          error;  /**< iota2 Error code for mapping */
} i2_error_t;               /**< HAL status and iota2 Mapping */
/** @} */ /* i2_error_t */

/**
 * @defgroup HAL_ERROR_MAPPING iota2 and HAL errors mapping table.
 * Maps a HAL error with corresponding iota2 error code.
 *
 * @{
 */
/** @brief HAL Errors mapping with iota2 Error codes */
static i2_error_t hal_error[MAX_NUM_ERROR] = {
  { HAL_OK,      I2_SUCCESS},   /**< HAL Success cade   */
  { HAL_ERROR,   I2_FAILURE},   /**< HAL Error code     */
  { HAL_BUSY,    I2_BUSY},      /**< HAL Busy code      */
  { HAL_TIMEOUT, I2_TIMEOUT},   /**< HAL Timeout code   */
};
/** @} */ /* HAL_ERROR_MAPPING */

/* Public functions ---------------------------------------------------------*/
/**
 * @brief   Get iota2 error corresponding to HAL error.
 * @details Returns iota2 error mapped with corresponding HAL error.
 *
 * @param[in] error       HAL error.
 * @return  iota2_error corresponding to HAL error.
 */
i2_error i2_get_hal_error(HAL_StatusTypeDef error)
{
  int32_t i;

  for (i = 0; i < MAX_NUM_ERROR; i++) {
    if (hal_error[i].status == error) {
      return hal_error[i].error;
    }
  }

  return I2_FAILURE;
}

/**
 * @brief   Delay routine.
 * @details Introduces a blocking delay of specified ticks.
 *
 * @param[in] tick        ticks for delay count.
 * @return  None.
 */
void i2_delay_tick(uint32_t tick)
{
  HAL_Delay(tick);
  return;
}

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
