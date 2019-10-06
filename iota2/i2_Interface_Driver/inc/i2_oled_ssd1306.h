/**
 * @author      iota square <i2>
 * @date        30-09-2019
 * @file        i2_oled_ssd1306.h
 *  _       _        ___
 * (_)     | |      |__ \.
 *  _  ___ | |_ __ _   ) |
 * | |/ _ \| __/ _` | / /
 * | | (_) | || (_| |/ /_
 * |_|\___/ \__\__,_|____|
 *
 * @license     GNU GPU v3
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
 * Display parameter specification.
 * Specifications for display used.
 *
 * @defgroup SSD1306_DISPLAY_SPEC
 */
#define SSD1306_DISPLAY_WIDTH       ( 128 )
#define SSD1306_DISPLAY_HEIGHT      ( 64 )
#define SSD1306_MAX_POLY_CORNERS    ( 10 )
#define SSD1306_SPI_TIMEOUT         ( 100 )
#define SSD1306_DATA_BUF_SIZE       (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8)

/* Degrees, Radians and other conversions */
#ifndef M_PI
#define M_PI                            ( 3.141593 )
#endif /* M_PI */
#define degreesToRadians(angleDegrees)  (double)(angleDegrees * M_PI / 180.0)
#define radiansToDegrees(angleRadians)  (double)(angleRadians * 180.0 / M_PI)
#define round(x)                        ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define SSD1306_COUNTOF(a)              ( sizeof(a) / sizeof( a[0] ) )
#define SSD1306_MIN(a,b)                (((a) < (b)) ? (a) : (b))
#define SSD1306_MAX(a,b)                (((a) > (b)) ? (a) : (b))
#define SSD1306_SWAP(a, b)              { int16_t t = a; a = b; b = t; }

/**
 * Display color specifications.
 * Specifications for available colors to display.
 *
 * @defgroup SSD1306_COLOR_SPEC
 */
#define SSD1306_BLACK                         ( 0 )
#define SSD1306_WHITE                         ( 1 )
#define SSD1306_INVERSE                       ( 2 )

/**
 * Display Multi layer support.
 * Enable displaying interlacing multiple layers.
 *
 * @defgroup SSD1306_MULTI_LAYER_SPEC
 */
#define SSD1306_MULTILAYER_SUPPORT            ( I2_DISABLE )
#define SSD1306_LAYER1                        ( 1 )
#define SSD1306_LAYER2                        ( 2 )

/**
 * Text display specifications.
 * Specifications for displaying text over screen.
 *
 * @defgroup SSD1306_TEXT_SPEC
 */
#define SSD1306_TEXT_MAX_LINE                 ( 4 )
#define SSD1306_TEXT_MAX_LINE_CHAR            ( 21 )
#define SSD1306_TEXT_SIZE_NORMAL              ( 0 )
#define SSD1306_TEXT_SIZE_LARGE               ( 1 )
#define SSD1306_TEXT_SIZE_EXTREA_LARGE        ( 2 )

/**
 * GLCD SSD1306 Command set.
 * Specifications for command set to initialize and control SSD1306.
 *
 * @defgroup SSD1306_CMD_SPEC
 */
#define SSD1306_CMD_SET_CONTRAST              0x81
#define SSD1306_CMD_DISPLAY_ALLON_RESUME      0xA4
#define SSD1306_CMD_DISPLAY_ALLON             0xA5
#define SSD1306_CMD_DISPLAY_NORMAL            0xA6
#define SSD1306_CMD_DISPLAY_INVERT            0xA7
#define SSD1306_CMD_DISPLAY_OFF               0xAE
#define SSD1306_CMD_DISPLAY_ON                0xAF
#define SSD1306_CMD_DISPLAY_SET_OFFSET        0xD3
#define SSD1306_CMD_SET_VCOM_DETECT           0xDB
#define SSD1306_CMD_SET_CLOCK_DIV_AND_FREQ    0xD5
#define SSD1306_CMD_SET_PRECHARGE             0xD9
#define SSD1306_CMD_SET_MULTIPLEX             0xA8
#define SSD1306_CMD_SET_LOW_COLUMN            0x00
#define SSD1306_CMD_SET_HIGH_COLUMN           0x10
#define SSD1306_CMD_SET_START_LINE            0x40
#define SSD1306_CMD_MEMORY_ADDRESSING_MODE    0x20
#define SSD1306_CMD_SET_COM_PINS              0xDA
#define SSD1306_CMD_COM_SCAN_INC              0xC0
#define SSD1306_CMD_COM_SCAN_DEC              0xC8
#define SSD1306_CMD_SEGMENT_REMAP             0xA0
#define SSD1306_CMD_CHARGE_PUMP               0x8D
#define SSD1306_CMD_EXTERNAL_VCC              0x01
#define SSD1306_CMD_SWITCH_CAP_VCC            0x02

/**
 * GLCD SSD1306 Configurations.
 * Specifications for options to configure SSD1306 in various modes.
 *
 * @defgroup SSD1306_CONFIG_SPEC
 */
#define SSD1306_OFFSET_NULL 0x00
#define SSD1306_DEFAULT_FREQ                  0x80
#define SSD1306_CUSTOM_FREQ                   0xF0 /* user defined disp_freq */
#define SSD1306_LINE_0                        0x00
#define SSD1306_CHARGE_PUMP_ENABLE            0x14
#define SSD1306_CHARGE_PUMP_DISABLE           0x10
#define SSD1306_HORIZONTAL_ADDRESSING_MODE    0x00 /* act like ks0108 */
#define SSD1306_VERTICAL_ADDRESSING_MODE      0x01
#define SSD1306_SEGMENT_REMAP_COL_0           0x00
#define SSD1306_SEGMENT_REMAP_COL_127         0x01
#define SSD1306_COM_PINS_SEQ                  0x02
#define SSD1306_COM_PINS_ALT                  0x12
#define SSD1306_COM_PINS_DISABLE_REMAP        0x02
#define SSD1306_COM_PINS_ENABLE_REMAP         0x22
#define SSD1306_EXTERNAL_VCC_CONTRAST         0x9F
#define SSD1306_SWITCH_CAP_VCC_CONTRAST       0xCF
#define SSD1306_PRECHARGE_PHASE1_DEFAULT      0x20
#define SSD1306_PRECHARGE_PHASE2_DEFAULT      0x02
#define SSD1306_PRECHARGE_PHASE1_CUSTOM       0xF0
#define SSD1306_PRECHARGE_PHASE2_CUSTOM       0x01
#define SSD1306_VCOM_0_65xVCC                 0x00
#define SSD1306_VCOM_0_77xVCC                 0x20
#define SSD1306_VCOM_0_83xVCC                 0x30
#define SSD1306_VCOM_DEFAULT                  0x40

/**
 * GLCD SSD1306 Scrolling options.
 * Specifications for controlling display scrolling.
 *
 * @defgroup SSD1306_SCROLL_SPEC
 */
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

/* Public Definitions --------------------------------------------------------*/
/**
 * Definition of a point on display.
 * Specifications for defining a point.
 *
 * @defgroup SSD1306_POINT_SPEC
 */
typedef struct {
  double x;
  double y;
} ssd1306_point_t;

/**
 * Definition of a polygon.
 * Specifications for defining a polygon.
 *
 * @defgroup SSD1306_POLYGON_SPEC
 */
typedef struct {
  ssd1306_point_t* point;
  int              points_count;
} ssd1306_poly_t;


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
                                uint8_t corner_name,
                                uint16_t color, uint16_t layer);
void ssd1306_fill_circle( int16_t y0, int16_t x0, int16_t r,
                          uint16_t color, uint16_t layer);
void ssd1306_fill_circle_helper(int16_t y0, int16_t x0, int16_t r,
                                uint8_t corner_name, int16_t delta,
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

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
