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
 * @file        i2_error.h
 * @brief       iota2 generic error codes.
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

/* Exported define -----------------------------------------------------------*/
/**
 * @defgroup I2_ERROR iota2 unified Error Codes.
 * Error codes to be used for uniform API maintenance.
 *
 * @{
 */
#define I2_SUCCESS              ( 0 )   /**< Successful executions            */
#define I2_FAILURE              ( 1 )   /**< Generic failures                 */
#define I2_TIMEOUT              ( 2 )   /**< Timeout occurred                 */
#define I2_BUSY                 ( 3 )   /**< System busy in other executions  */
#define I2_INVALID_PARAM        ( 4 )   /**< Invalid input parameters         */
#define I2_NOT_AVAILABLE        ( 5 )   /**< Feature or option not available  */
#define I2_NOT_IMPLEMENTED      ( 6 )   /**< Feature not implemented yet      */
#define I2_NOT_SUPPORTED        ( 7 )   /**< Feature not supported            */
#define I2_READY_TO_SEND        ( 8 )   /**< Module is ready to send data     */
#define I2_DATA_IS_CURRENT      ( 9 )   /**< Received data not processed yet  */
#define I2_SSL_CON_CLOSED       ( 10 )  /**< Connection Closed                */
/** @} */ /* I2_ERROR */

/* Exported constants --------------------------------------------------------*/
typedef int32_t i2_error;   /**< Generic error code to use among i2 projects  */

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
