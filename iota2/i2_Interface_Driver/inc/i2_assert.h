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
 * @file        i2_assert.h
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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported define -----------------------------------------------------------*/
/**
 * @defgroup I2_ASSERT Assert definition.
 * Call for assert function.
 *
 * @{
 */
#if defined(ENABLE_ASSERT)
/** @brief Assert Definition when enabled */
#define ASSERT(x) do { if (!(x)) assert(0); } while (0)
#else
/** @brief Assert Definition when disabled */
#define ASSERT(x) do { ; } while (0);
#endif
/** @} */ /* I2_ASSERT */

/* Exported functions ------------------------------------------------------- */
void i2_assert(int32_t good);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
