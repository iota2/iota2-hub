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
 * @date        07-09-2019
 * @file        main.h
 * @brief       iota2 HUB application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "FreeRTOS.h"
#include "task.h"

/* HAL Interface Drivers -----------------------------------------------------*/
#include "i2_stm32f4xx_hal_clock.h"
#include "i2_stm32f4xx_hal_gpio.h"
#include "i2_stm32f4xx_hal_rtc.h"
#include "i2_stm32f4xx_hal_spi.h"
#include "i2_stm32f4xx_hal_uart.h"

/* Drivers -------------------------------------------------------------------*/
#include "i2_fifo.h"
#include "i2_led.h"

/* HMI Interface -------------------------------------------------------------*/
#include "i2_font5x7.h"
#include "i2_oled_ssd1306.h"

/**
 * @defgroup I2_HUB_VERSION iota2 Firmware version.
 * This is the IOTA2-HUB SW version in format MAJOR.MINOR.HOTFIX
 * Note that Hotfix is 16 bit and Minor and Major is 8 bit.
 *
 * @{
 */
#define IOTA2_HUB_MAJOR_VERSION     ( 1 )       /**< A value between 0-255    */
#define IOTA2_HUB_MINOR_VERSION     ( 0 )       /**< A value between 0-255    */
#define IOTA2_HUB_HOTFIX_VERSION    ( 0 )       /**< A value between 0-65535  */
/** @} */ /* I2_HUB_VERSION */

/* Exported define -----------------------------------------------------------*/
#define HUB_taskPritorityUSER   configMIN_PRIORITIES  /**< Task Min Priority  */
#define HUB_taskStckDepthUSER   ( 10 )                /**< Generic Task Depth */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
