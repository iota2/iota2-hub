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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include <i2_stm32f4xx_hal_common.h>

/* Public functions --------------------------------------------------------- */
/*
 * Return true if LSE is on and used for RTC.
 */
bool i2_is_lse_on(void);

/*
 * Use external clocks. HSE drives PLL. LSE drives RTC.
 */
void i2_hse_lse_clock_config(void);

/*
 * Use internal clocks. HSI drives PLL. LSI drives RTC.
 */
void i2_hsi_lsi_clock_config(void);

/*
 * Use external / internal clocks. HSE drives PLL. LSI drives RTC.
 */
void i2_hse_lsi_clock_config(void);

/*
 * Use external / internal clocks. HSI drives PLL. LSE drives RTC.
 */
void i2_hsi_lse_clock_config(void);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/