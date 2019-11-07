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
 * @file        i2_oled_ssd1306.h
 * @brief       OLED SSD1306 interface.
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
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

#include "i2_stm32f4xx_hal_common.h"
#include "i2_stm32f4xx_hal_spi.h"

/* Global Defines ------------------------------------------------------------*/
/**
 * @defgroup SSD1306_DISPLAY_SPEC SSD1306 display parameter specification.
 * Specifications for SSD1306 display used.
 *
 * @{
 */
#define SSD1306_DISPLAY_WIDTH     ( 128 ) /**< Display width configuration    */
#define SSD1306_DISPLAY_HEIGHT    ( 64 )  /**< DIsplay height configuration   */
#define SSD1306_MAX_POLY_CORNERS  ( 10 )  /**< Max num of corners in polygon  */
#define SSD1306_SPI_TIMEOUT       ( 100 ) /**< default timeout for SPI bus    */
/** Size of buffer required, according to configured width and height         */
#define SSD1306_DATA_BUF_SIZE     (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8)
/** @} */ /* SSD1306_DISPLAY_SPEC */

/* Degrees, Radians and other conversions */
#ifndef M_PI
#define M_PI                            ( 3.141593 )  /**< It's PIE */
#endif /* M_PI */
/** @brief Degree to Radians conversion */
#define degreesToRadians(angleDegrees)  (double)(angleDegrees * M_PI / 180.0)
/** @brief Radians to degrees conversion */
#define radiansToDegrees(angleRadians)  (double)(angleRadians * 180.0 / M_PI)
/** @brief Calculating integer round off */
#define round(x)                        ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
/** @brief Count of calculation utility */
#define SSD1306_COUNTOF(a)              ( sizeof(a) / sizeof( a[0] ) )
/** @brief Minimum value calculation utility */
#define SSD1306_MIN(a,b)                (((a) < (b)) ? (a) : (b))
/** @brief Maximum value calculation utility */
#define SSD1306_MAX(a,b)                (((a) > (b)) ? (a) : (b))
/** @brief Utility for swaping two integers */
#define SSD1306_SWAP(a, b)              { int16_t t = a; a = b; b = t; }

/**
 * @defgroup SSD1306_COLOR_SPEC Display color specifications.
 * Specifications for available colors to display.
 *
 * @{
 */
#define SSD1306_BLACK               ( 0 ) /**< Black (SET) pixel for screen   */
#define SSD1306_WHITE               ( 1 ) /**< White (CLEAR) pixel for screen */
#define SSD1306_INVERSE             ( 2 ) /**< Inverse screen pixel           */
/** @} */ /* SSD1306_COLOR_SPEC */

/**
 * @defgroup SSD1306_MULTI_LAYER_SPEC SSD1306 display multi-layer support.
 * Enable displaying interlacing multiple layers.
 *
 * @{
 */
#define SSD1306_MULTILAYER_SUPPORT  ( I2_DISABLE )  /**< Multlayer Options    */
#define SSD1306_LAYER1              ( 1 )           /**< Display layer 1      */
#define SSD1306_LAYER2              ( 2 )           /**< Display layer 2      */
/** @} */ /* SSD1306_MULTI_LAYER_SPEC */

/**
 * @defgroup SSD1306_TEXT_SPEC SSD1306 text display specifications.
 * Specifications for displaying text over screen.
 *
 * @{
 */
#define SSD1306_TEXT_MAX_LINE           ( 4 )   /**< Max number of lines      */
#define SSD1306_TEXT_MAX_LINE_CHAR      ( 21 )  /**< Max characters per line  */
#define SSD1306_TEXT_SIZE_NORMAL        ( 0 )   /**< Font size normal         */
#define SSD1306_TEXT_SIZE_LARGE         ( 1 )   /**< Font size medium         */
#define SSD1306_TEXT_SIZE_EXTREA_LARGE  ( 2 )   /**< Font size large          */
/** @} */ /* SSD1306_TEXT_SPEC */

/**
 * @defgroup SSD1306_CMD_SPEC SSD1306 command set.
 * Specifications for command set to initialize and control SSD1306.
 *
 * @{
 */
#define SSD1306_CMD_SET_CONTRAST              0x81  /**< Display contrast     */
#define SSD1306_CMD_DISPLAY_ALLON_RESUME      0xA4  /**< Resume on ALL on     */
#define SSD1306_CMD_DISPLAY_ALLON             0xA5  /**< All on display       */
#define SSD1306_CMD_DISPLAY_NORMAL            0xA6  /**< Regular pixels disp  */
#define SSD1306_CMD_DISPLAY_INVERT            0xA7  /**< Inverted pixels disp */
#define SSD1306_CMD_DISPLAY_OFF               0xAE  /**< Turn display off     */
#define SSD1306_CMD_DISPLAY_ON                0xAF  /**< Turn display on      */
#define SSD1306_CMD_DISPLAY_SET_OFFSET        0xD3  /**< Set display offset   */
#define SSD1306_CMD_SET_VCOM_DETECT           0xDB  /**< Voltage detection    */
#define SSD1306_CMD_SET_CLOCK_DIV_AND_FREQ    0xD5  /**< Clock configurations */
#define SSD1306_CMD_SET_PRECHARGE             0xD9  /**< Pre-charging setting */
#define SSD1306_CMD_SET_MULTIPLEX             0xA8  /**< Mutiplexing settings */
#define SSD1306_CMD_SET_LOW_COLUMN            0x00  /**< Lower column setting */
#define SSD1306_CMD_SET_HIGH_COLUMN           0x10  /**< Higher column setting*/
#define SSD1306_CMD_SET_START_LINE            0x40  /**< COnfigure start line */
#define SSD1306_CMD_MEMORY_ADDRESSING_MODE    0x20  /**< Addressing mode      */
#define SSD1306_CMD_SET_COM_PINS              0xDA  /**< Set COM pins         */
#define SSD1306_CMD_COM_SCAN_INC              0xC0  /**< Incremental COM scan */
#define SSD1306_CMD_COM_SCAN_DEC              0xC8  /**< Decremental COM scan */
#define SSD1306_CMD_SEGMENT_REMAP             0xA0  /**< Segment remaping     */
#define SSD1306_CMD_CHARGE_PUMP               0x8D  /**< Charge pump config   */
#define SSD1306_CMD_EXTERNAL_VCC              0x01  /**< External Vcc config  */
#define SSD1306_CMD_SWITCH_CAP_VCC            0x02  /**< VCC Capacitor setting*/
/** @} */ /* SSD1306_CMD_SPEC */

/**
 * @defgroup SSD1306_CONFIG_SPEC SSD1306 configurations.
 * Specifications for options to configure SSD1306 in various modes.
 *
 * @{
 */
#define SSD1306_OFFSET_NULL                   0x00  /**< Set offset to NULL   */
#define SSD1306_DEFAULT_FREQ                  0x80  /**< Default frequency    */
#define SSD1306_CUSTOM_FREQ                   0xF0  /**< user defined Dispfreq*/
#define SSD1306_LINE_0                        0x00  /**< Display Line 0       */
#define SSD1306_CHARGE_PUMP_ENABLE            0x14  /**< Enable charge pump   */
#define SSD1306_CHARGE_PUMP_DISABLE           0x10  /**< Disable charge pump  */
#define SSD1306_HORIZONTAL_ADDRESSING_MODE    0x00  /**< Act like ks0108      */
#define SSD1306_VERTICAL_ADDRESSING_MODE      0x01  /**< Use vertical add mode*/
#define SSD1306_SEGMENT_REMAP_COL_0           0x00  /**< First column Remap   */
#define SSD1306_SEGMENT_REMAP_COL_127         0x01  /**< Last column remapping*/
#define SSD1306_COM_PINS_SEQ                  0x02  /**< Pins sequence config */
#define SSD1306_COM_PINS_ALT                  0x12  /**< Config alternate pins*/
#define SSD1306_COM_PINS_DISABLE_REMAP        0x02  /**< Disable remapping    */
#define SSD1306_COM_PINS_ENABLE_REMAP         0x22  /**< Enable remapping     */
#define SSD1306_EXTERNAL_VCC_CONTRAST         0x9F  /**< Set Ext VCC contrast */
#define SSD1306_SWITCH_CAP_VCC_CONTRAST       0xCF  /**< Switch VCC capacitor */
#define SSD1306_PRECHARGE_PHASE1_DEFAULT      0x20  /**< Default precharge PH1*/
#define SSD1306_PRECHARGE_PHASE2_DEFAULT      0x02  /**< Default precharge PH2*/
#define SSD1306_PRECHARGE_PHASE1_CUSTOM       0xF0  /**< Custom precharge PH1 */
#define SSD1306_PRECHARGE_PHASE2_CUSTOM       0x01  /**< Custom precharge PH2 */
#define SSD1306_VCOM_0_65xVCC                 0x00  /**< Set VCOM VCC to 0.65V*/
#define SSD1306_VCOM_0_77xVCC                 0x20  /**< Set VCOM VCC to 0.77V*/
#define SSD1306_VCOM_0_83xVCC                 0x30  /**< Set VCOM VCC to 0.83V*/
#define SSD1306_VCOM_DEFAULT                  0x40  /**< Set VCOM to default  */
/** @} */ /* SSD1306_CONFIG_SPEC */

/**
 * @defgroup SSD1306_SCROLL_SPEC SSD1306 scrolling options.
 * Specifications for controlling display scrolling.
 *
 * @{
 */
/** @brief Activate display scrolling */
#define SSD1306_ACTIVATE_SCROLL                         0x2F
/** @brief Deactivate display scrolling */
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
/** @brief Enable scrolling in vertical direction */
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
/** @brief Enable horizontal scrolling in right direction */
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
/** @brief Enable horizontal scrolling in left direction */
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
/** @brief Enable Vertical scrolling with horizontal in right direction */
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
/** @brief Enable Vertical scrolling with horizontal in left direction */
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A
/** @} */ /* SSD1306_SCROLL_SPEC */

/* Public Definitions --------------------------------------------------------*/
/**
 * @defgroup SSD1306_POINT_SPEC SSD1306 display point specification.
 * Specifications for defining a point.
 *
 * @{
 */
/** @brief SSD1306 display point */
typedef struct {
  double x;   /**< x-coordinate on display */
  double y;   /**< u-coordinate on display */
} ssd1306_point_t;
/** @} */ /* SSD1306_POINT_SPEC */

/**
 * @defgroup SSD1306_POLYGON_SPEC SSD1306 polygon specification.
 * Specifications for defining a polygon.
 *
 * @{
 */
/** @brief SSD1306 display polygon */
typedef struct {
  ssd1306_point_t* point;      /**< Polygon's point coordinates   */
  int              corners;    /**< Numbers of corners in polygon */
} ssd1306_poly_t;
/** @} */ /* SSD1306_POLYGON_SPEC */

/* Public functions ----------------------------------------------------------*/
void ssd1306_init(uint8_t vcc_state);
void ssd1306_write_byte(uint8_t byte);
void ssd1306_write_buffer(uint8_t* buff,uint16_t bytes_to_write);
void ssd1306_refresh ( void );
void ssd1306_turn_on(void);
void ssd1306_turn_off(void);
void ssd1306_draw_pixel(int16_t y, int16_t x, uint16_t color, uint16_t layer);
void ssd1306_clear_pixel(int16_t y, int16_t x);
uint8_t ssd1306_get_pixel(int16_t x, int16_t y);
void ssd1306_clear_char(int16_t y, int16_t x, uint8_t size);
void ssd1306_clear_screen(uint16_t layer);
void ssd1306_draw_line( int16_t y0, int16_t x0, int16_t y1, int16_t x1,
                        uint16_t color, uint16_t layer);
void ssd1306_draw_fast_vline( int16_t y, int16_t x, int16_t h,
                              uint16_t color, uint16_t layer);
void ssd1306_draw_fast_hline( int16_t y, int16_t x, int16_t w,
                              uint16_t color, uint16_t layer);
void ssd1306_draw_polygon(ssd1306_poly_t* poly,
                          int16_t y, int16_t x,
                          uint16_t color, uint16_t layer);
void ssd1306_fill_polygon(ssd1306_poly_t* poly, double y, double x,
                          uint16_t color, uint16_t layer);
void ssd1306_fill_screen(uint16_t color, uint16_t layer);
void ssd1306_draw_circle( uint8_t y0, uint8_t x0, uint8_t r,
                          uint16_t color, uint16_t layer);
void ssd1306_draw_circle_helper(int16_t y0, int16_t x0, int16_t r,
                                uint8_t corner, uint16_t color, uint16_t layer);
void ssd1306_fill_circle( int16_t y0, int16_t x0, int16_t r,
                          uint16_t color, uint16_t layer);
void ssd1306_fill_circle_helper(int16_t y0, int16_t x0, int16_t r,
                                uint8_t corner, int16_t delta,
                                uint16_t color, uint16_t layer);
void ssd1306_draw_rectangle(int16_t y, int16_t x, int16_t h, int16_t w,
                            uint16_t color, uint16_t layer);
void ssd1306_fill_rectangle(uint8_t y, uint8_t x, uint8_t h, uint8_t w,
                            uint16_t color, uint16_t layer);
void ssd1306_draw_bitmap( int16_t y, int16_t x, uint8_t *bitmap,
                          int16_t h, int16_t w, uint16_t color, uint16_t layer);
void ssd1306_draw_char( int16_t y, int16_t x, uint16_t c, uint8_t size,
                        uint16_t color, uint16_t layer);
void ssd1306_draw_char_loc( int16_t y, int16_t x, uint16_t c, uint8_t size,
                            uint16_t color, uint16_t layer);
void ssd1306_draw_string( int16_t y, int16_t x, char *txt, uint8_t size,
                          uint16_t color, uint16_t layer);
void ssd1306_draw_string_loc( int16_t y, int16_t x, char *text, uint8_t size,
                              uint16_t color, uint16_t layer);
void ssd1306_type_string_loc( int16_t y, int16_t x, char *text, uint8_t size,
                              uint8_t delay, uint16_t color, uint16_t layer);
void ssd1306_shift_frame_buffer( uint16_t height, uint16_t direction);
void ssd1306_mix_frame_buffer(void);
void ssd1306_delay(__IO uint32_t nCount);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
