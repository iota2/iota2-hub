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
 * @date        30-09-2019
 * @file        i2_font5x7.h
 * @brief       Generic 5x7 font.
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

/* Public Definitions --------------------------------------------------------*/

/**
 * @defgroup i2_font5x7_custom_char Font indexing for user defined characters.
 * Indexing for custom characters in 5x7 Font list.
 *
 * @{
 */
#define FONT5x7_ARROW_RIGHT       ( '>' ) /**< Index of right arrow bitmap    */
#define FONT5x7_ARROW_UP_DOWN     ( 255 ) /**< Index of up down arrow bitmap  */
#define FONT5x7_ALARM_BELL        ( 256 ) /**< Index of bell bitmap           */
#define FONT5x7_HEART_SMALL       ( 257 ) /**< Index of small heart bitmap    */
#define FONT5x7_HEART_LARGE       ( 258 ) /**< Index of large heart bitmap    */
/** @} */ /* i2_font5x7_custom_char */

extern const unsigned char i2_font5x7[];  /**< 8514oem 5x7 character set      */

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
