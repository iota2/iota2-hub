/**
 * @date        07-09-2019
 * @author      iota square <i2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"

/*
 * This is the IOTA2-HUB SW version in format MAJOR.MINOR.HOTFIX
 * Note that Hotfix is 16 bit and Minor and Major is 8 bit.
 */
#define IOTA2_CONN_EMB_MAJOR_VERSION_NUMBER     ( 1 ) //a value between 0-255
#define IOTA2_CONN_EMB_MINOR_VERSION_NUMBER     ( 0 ) //a value between 0-255
#define IOTA2_CONN_EMB_HOTFIX_VERSION_NUMBER    ( 0 ) //a value between 0-65535

/* Exported define -----------------------------------------------------------*/
#define LED1_PIN                                GPIO_Pin_8
#define LED2_PIN                                GPIO_Pin_9

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define HUB_taskPritorityUSER                   configMIN_PRIORITIES

#define HUB_taskStckDepthUSER                   ( 10 )
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/

