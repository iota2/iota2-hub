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
 * @file        i2_oled_ssd1306.c
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
/* Includes ------------------------------------------------------------------*/
#include "i2_oled_ssd1306.h"
#include "i2_font5x7.h"

/**
 * @defgroup SSD1306_HW_SPEC SSD1306 Hardware Pins Definition.
 * Pin configurations of SSD1306 LCD module.
 *
 * @{
 *
 *    | PIN No. | SSD1306 PIN   | PIN ID    |  MCU  |
 *    |---------|---------------|-----------|-------|
 *    |    1    | GND           |  0 V      |  GND  |
 *    |    2    | VCC           |  3.3 V    |  VCC  |
 *    |    3    | CLOCK         |  C        |  SCK  |
 *    |    4    | DATA          |  D        |  MOSI |
 *    |    5    | RESET         |  ST       |  RST  |
 *    |    6    | DATA/CMD      |  D/C      |  CMD  |
 *    |    7    | CHIP SELECT   |  CS       |  NSS  |
 */
#define SSD1306_PIN_CS            GPIOG, GPIO_PIN_14  /**< Chip Select Pin    */
#define SSD1306_PIN_DC            GPIOH, GPIO_PIN_2   /**< Data / CMD Pin     */
#define SSD1306_PIN_RST           GPIOC, GPIO_PIN_7   /**< Display Reset Pin  */
/** @} */ /* SSD1306_HW_SPEC */

/**
 * @defgroup SSD1306_SPI_SPEC SSD1306 SPI instance.
 * SPI interface and Chip Select definition.
 *
 * @{
 */
/** @brief ssd1306 display control instance */
static i2_spi_inst_t ssd1306 = {
  "ssd1306", "SPI1", { "ssd1306_CS", SSD1306_PIN_CS }
};
/** @} */ /* SSD1306_SPI_SPEC */

/**
 * @defgroup SSD1306_PIN_SPEC SSD1306 GPIO pins definition.
 * DC and RST Pins Definitions for SSD1306 Interface.
 *
 * @{
 */
/** @brief Display (DC) pin GPIO instance initialization */
static i2_gpio_inst_t ssd1306_DC  = { "ssd1306_DC",   SSD1306_PIN_DC };
/** @brief Display (RST) GPIO instance initialization */
static i2_gpio_inst_t ssd1306_RST = { "ssd1306_RST",  SSD1306_PIN_RST };
/** @} */ /* SSD1306_PIN_SPEC */

/**
 * @defgroup SSD1306_LAYER_BUFFER_SPEC SSD1306  display buffers.
 * This will act as LAYER1 and LAYER2 for display.
 *
 * @{
 */
/** @brief Layer 1 (or default) display buffer */
static uint8_t ssd1306_buffer_layer1[SSD1306_DISPLAY_WIDTH *     \
                              SSD1306_DISPLAY_HEIGHT / 8];
#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
/** @brief Layer 2 display buffer when multi-layer display is enabled */
static uint8_t ssd1306_buffer_layer2[SSD1306_DISPLAY_WIDTH *    \
                                     SSD1306_DISPLAY_HEIGHT / 8];
#endif /* SSD1306_MULTILAYER_SUPPORT */
/** @} */ /* SSD1306_LAYER_BUFFER_SPEC */

/**
 * @brief   SPI write.
 * @details Writes a single byte to SPI.
 *
 * @param[in] byte      data to send.
 * @return  None.
 */
void ssd1306_write_byte(uint8_t byte)
{
  if (i2_spi_tx(&ssd1306, I2_SPI_DATA_WIDTH_8BIT, I2_SPI_CLK_20_MHZ,
      I2_SPI_MODE_0, I2_SPI_MSBIT_FIRST, &byte, 1,
      SSD1306_SPI_TIMEOUT) == I2_SUCCESS) {
    i2_assert(0);
  }
}

/**
 * @brief   SPI buffer write.
 * @details Writes data buffer over SPI.
 *
 * @param[in] buff             Buffer to send.
 * @param[in] bytes_to_write   Number of bytes.
 * @return  None.
 */
void ssd1306_write_buffer(uint8_t* buff, uint16_t bytes_to_write)
{
  if (i2_spi_tx(&ssd1306, I2_SPI_DATA_WIDTH_8BIT, I2_SPI_CLK_20_MHZ,
      I2_SPI_MODE_0, I2_SPI_MSBIT_FIRST, buff, bytes_to_write,
      SSD1306_SPI_TIMEOUT) == I2_SUCCESS) {
    i2_assert(0);
  }
}

/**
 * @brief   Writes a command to display.
 * @details Asserts DC pin in command mode and writes SPI byte.
 *
 * @param[in] c     Command to write.
 * @return  None.
 */
#define SSD1306_CMD(c)    do {  \
                            i2_gpio_set(&ssd1306_DC, I2_LOW); \
                            ssd1306_write_byte( c );  \
                          } while (0);

/**
 * @brief   Writes a data byte to display.
 * @details Asserts DC pin in data mode and writes SPI byte.
 *
 * @param[in] d     Data byte to write.
 * @return  None.
 */
#define SSD1306_DATA(d)   do {  \
                            i2_gpio_set(&ssd1306_DC, I2_HIGH);  \
                            ssd1306_write_byte( d );  \
                          } while (0);  \

/**
 * @brief   Initializes SSD1306 LCD display.
 * @details Initialize Hardware for SPI and GPIO also LCD reset sequence.
 *
 * @param[in] vcc_state   Display VCC connection state.
 * @return  None.
 */
void ssd1306_init(uint8_t vcc_state)
{
  if (i2_gpio_is_valid(&ssd1306_DC)) {
    i2_gpio_config_out(&ssd1306_DC, false);
  }

  if (i2_gpio_is_valid(&ssd1306_RST)) {
    i2_gpio_config_out(&ssd1306_RST, false);
  }

  /* Initialize SPI interface */
  i2_spi_init(&ssd1306);

  /* Start Initialization by bringing RESET HIGH */
  i2_gpio_set(&ssd1306_RST, I2_HIGH);
  /* VDD (3.3V) goes high at start, lets just chill for a ms */
  ssd1306_delay(2);

  /* Bring RESET LOW */
  i2_gpio_set(&ssd1306_RST, I2_LOW);
  ssd1306_delay(1);

  /* Bring RESET HIGH */
  i2_gpio_set(&ssd1306_RST, I2_HIGH);
  ssd1306_delay(2);

  /* Initialization sequence @ref SSD1306_CMD_SPEC */
  SSD1306_CMD(SSD1306_CMD_DISPLAY_OFF);
  SSD1306_CMD(SSD1306_CMD_SET_CLOCK_DIV_AND_FREQ);
  SSD1306_CMD(SSD1306_CUSTOM_FREQ);
  SSD1306_CMD(SSD1306_CMD_SET_MULTIPLEX);
  SSD1306_CMD(SSD1306_DISPLAY_HEIGHT - 1);
  SSD1306_CMD(SSD1306_CMD_DISPLAY_SET_OFFSET);
  SSD1306_CMD(SSD1306_OFFSET_NULL);
  SSD1306_CMD(SSD1306_CMD_SET_START_LINE | SSD1306_LINE_0);
  SSD1306_CMD(SSD1306_CMD_CHARGE_PUMP);
  if (vcc_state == SSD1306_CMD_EXTERNAL_VCC) {
    SSD1306_CMD(SSD1306_CHARGE_PUMP_DISABLE);
  } else {
    SSD1306_CMD(SSD1306_CHARGE_PUMP_ENABLE);
  }

  SSD1306_CMD(SSD1306_CMD_MEMORY_ADDRESSING_MODE);
  SSD1306_CMD(SSD1306_HORIZONTAL_ADDRESSING_MODE);
  SSD1306_CMD(SSD1306_CMD_SEGMENT_REMAP | SSD1306_SEGMENT_REMAP_COL_127);
  SSD1306_CMD(SSD1306_CMD_COM_SCAN_DEC);
  SSD1306_CMD(SSD1306_CMD_SET_COM_PINS);
  SSD1306_CMD(SSD1306_COM_PINS_ALT | SSD1306_COM_PINS_DISABLE_REMAP);
  SSD1306_CMD(SSD1306_CMD_SET_CONTRAST);
  if (vcc_state == SSD1306_CMD_EXTERNAL_VCC) {
    SSD1306_CMD(SSD1306_EXTERNAL_VCC_CONTRAST);
  } else {
    SSD1306_CMD(SSD1306_SWITCH_CAP_VCC_CONTRAST);
  }

  SSD1306_CMD(SSD1306_CMD_SET_PRECHARGE);
  if (vcc_state == SSD1306_CMD_EXTERNAL_VCC) {
    SSD1306_CMD(SSD1306_PRECHARGE_PHASE1_DEFAULT |
                SSD1306_PRECHARGE_PHASE2_DEFAULT);
  } else {
    SSD1306_CMD(SSD1306_PRECHARGE_PHASE1_CUSTOM |
                SSD1306_PRECHARGE_PHASE2_CUSTOM);
  }

  SSD1306_CMD(SSD1306_CMD_SET_VCOM_DETECT);
  SSD1306_CMD(SSD1306_VCOM_DEFAULT);
  SSD1306_CMD(SSD1306_CMD_DISPLAY_ALLON_RESUME);
  SSD1306_CMD(SSD1306_CMD_DISPLAY_NORMAL);
  SSD1306_CMD(SSD1306_DEACTIVATE_SCROLL);
  ssd1306_turn_on();
}

/**
 * @brief   Renders the contents of the pixel buffer on the LCD.
 * @details Picks data from the two layers buffer.
 *
 * @return  None.
 */
void ssd1306_refresh(void)
{
  SSD1306_CMD(SSD1306_CMD_SET_LOW_COLUMN  | 0x0);    /* Low Column = 0 */
  SSD1306_CMD(SSD1306_CMD_SET_HIGH_COLUMN | 0x0);    /* High Column = 0 */
  SSD1306_CMD(SSD1306_CMD_SET_START_LINE  | 0x0);    /* Line Number = 0*/

  i2_gpio_set(&ssd1306_DC, I2_HIGH);
#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
  ssd1306_mix_frame_buffer();
  ssd1306_write_buffer( (uint8_t *)ssd1306_buffer_layer2,
                        sizeof(ssd1306_buffer_layer2));
#else
  ssd1306_write_buffer( (uint8_t *)ssd1306_buffer_layer1,
                        sizeof(ssd1306_buffer_layer1));
#endif /* SSD1306_MULTILAYER_SUPPORT */
}

/**
 * @brief   Enable the OLED panel.
 * @details Turn on display pixels.
 *
 * @return  None.
 */
void ssd1306_turn_on(void)
{
  SSD1306_CMD(SSD1306_CMD_DISPLAY_ON);
}

/**
 * @brief   Disable the OLED panel.
 * @details Turn off display pixels.
 *
 * @return  None.
 */
void ssd1306_turn_off(void)
{
  SSD1306_CMD(SSD1306_CMD_DISPLAY_OFF);
}

/**
 * @brief   Draws a single pixel in image buffer.
 * @details Updates corresponding Layer buffer with pixel value.
 *
 * @param[in] y         Vertical (y) position (0..63).
 * @param[in] x         Horizontal (x) position (0..127).
 * @param[in] color     Color of line to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer     Layer to draw line @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_pixel(int16_t y, int16_t x, uint16_t color, uint16_t layer)
{
  if ((x >= SSD1306_DISPLAY_WIDTH) ||
      (x < 0) ||
      (y >= SSD1306_DISPLAY_HEIGHT) ||
      (y < 0)) {
    return;
  }

  if (layer & SSD1306_LAYER1) {
    switch (color) {
    case SSD1306_WHITE :
      ssd1306_buffer_layer1[x+ (y/8)*SSD1306_DISPLAY_WIDTH] |=  (1 << (y&7));
      break;
    case SSD1306_BLACK :
      ssd1306_buffer_layer1[x+ (y/8)*SSD1306_DISPLAY_WIDTH] &= ~(1 << (y&7));
      break;
    case SSD1306_INVERSE :
      ssd1306_buffer_layer1[x+ (y/8)*SSD1306_DISPLAY_WIDTH] ^=  (1 << (y&7));
      break;
    }
  }

#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
   if (layer & SSD1306_LAYER2) {
    switch (color) {
    case SSD1306_WHITE :
      ssd1306_buffer_layer2[x+ (y/8)*SSD1306_DISPLAY_WIDTH] |=  (1 << (y&7));
      break;
    case SSD1306_BLACK :
      ssd1306_buffer_layer2[x+ (y/8)*SSD1306_DISPLAY_WIDTH] &= ~(1 << (y&7));
      break;
    case SSD1306_INVERSE :
      ssd1306_buffer_layer2[x+ (y/8)*SSD1306_DISPLAY_WIDTH] ^=  (1 << (y&7));
      break;
    }
  }
#endif /* SSD1306_MULTILAYER_SUPPORT */

#if 0
   ssd1306_refresh();
#endif  /* 0 */
}

/**
 * @brief   Gets the value (1 or 0) of the specified pixel from the buffer.
 * @details Read corresponding Layer buffer with pixel value.
 *
 * @param[in] y     Vertical (y) position (0..63).
 * @param[in] x     Horizontal (x) position (0..127).
 * @return  1 if the pixel is set, 0 if clear.
 */
uint8_t ssd1306_get_pixel(int16_t y, int16_t x)
{
  if ((x >= SSD1306_DISPLAY_WIDTH) ||
      (x < 0) ||
      (y >= SSD1306_DISPLAY_HEIGHT) ||
      (y < 0)) {
    return 0;
  }

  return ssd1306_buffer_layer1[ x+ (y/8)*SSD1306_DISPLAY_WIDTH] &   \
                                (1 << y%8) ? 1 : 0;
}

/**
 * @brief   Clears a single pixel in image buffer.
 * @details Updates corresponding Layer buffer with pixel value.
 *
 * @param[in] y     Vertical (y) position (0..63).
 * @param[in] x     Horizontal (x) position (0..127).
 * @return  None.
 */
void ssd1306_clear_pixel(int16_t y, int16_t x)
{
  if ((x >= SSD1306_DISPLAY_WIDTH) ||
      (x < 0) ||
      (y >= SSD1306_DISPLAY_HEIGHT) ||
      (y < 0)) {
    return;
  }

#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
  ssd1306_buffer_layer1[x+ (y/8) * SSD1306_DISPLAY_WIDTH] &= ~(1 << y%8);
  ssd1306_buffer_layer2[x+ (y/8) * SSD1306_DISPLAY_WIDTH] &= ~(1 << y%8);
#else
  ssd1306_buffer_layer1[x+ (y/8) * SSD1306_DISPLAY_WIDTH] &= ~(1 << y%8);
#endif /* SSD1306_MULTILAYER_SUPPORT */
}

/**
 * @brief   Clears a character from screen.
 * @details Updates corresponding Layer buffer with pixel value.
 *
 * @param[in] y         Vertical (y) position (0..63).
 * @param[in] x         Horizontal (x) position (0..127).
 * @param[in] size      Character size to clear @ref SSD1306_TEXT_SPEC.
 * @return  None.
 */
void ssd1306_clear_char(int16_t y, int16_t x, uint8_t size)
{
  uint8_t i = 0;
  uint8_t j = 0;
  uint16_t width = (6 * size);
  uint16_t height = (10 * size);

  if (size == 1) {
    x *= width;
    y *= height;
    if ((x >= SSD1306_DISPLAY_WIDTH) ||
        (x < 0) ||
        (y >= SSD1306_DISPLAY_HEIGHT) ||
        (y < 0)) {
      return;
    }

    for (i=0; i<6; i++) {
      for (j=0; j<8; j++) {
        ssd1306_clear_pixel((y+j), (x+i));
      }
    }
  }
  if (size == 2) {
    x *= 11;
    y *= 20;
    if ((x >= SSD1306_DISPLAY_WIDTH) ||
        (x < 0) ||
        (y >= SSD1306_DISPLAY_HEIGHT) ||
        (y < 0)) {
      return;
    }

    for (i=0; i<12; i++) {
      for (j=0; j<20; j++) {
#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
          ssd1306_buffer_layer1[(x+i)+ ((y+j)/8)*SSD1306_DISPLAY_WIDTH] &= 0x00;
          ssd1306_buffer_layer2[(x+i)+ ((y+j)/8)*SSD1306_DISPLAY_WIDTH] &= 0x00;
#else
          ssd1306_buffer_layer1[(x+i)+ ((y+j)/8)*SSD1306_DISPLAY_WIDTH] &= 0x00;
#endif /* SSD1306_MULTILAYER_SUPPORT */
      }
    }
  }
}

/**
 * @brief   Clears the screen.
 * @details Clears corresponding Layer buffer.
 *
 * @param[in] layer  Required layer to be cleared @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_clear_screen(uint16_t layer)
{
  if (layer & SSD1306_LAYER1) {
    memset(ssd1306_buffer_layer1, 0x00, 1024);
  }
#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
  if (layer & SSD1306_LAYER2) {
    memset(ssd1306_buffer_layer2, 0x00, 1024);
  }
#endif /* SSD1306_MULTILAYER_SUPPORT */
  ssd1306_refresh();
}

/**
 * @brief   Draws a straight line from point A to point B.
 * @details Draws a line between two coordinates.
 *
 * @param[in] y0      Vertical (y) position for beginning point A.
 * @param[in] x0      Horizontal (x) position for beginning point A.
 * @param[in] y1      Vertical (y) position for terminating point B.
 * @param[in] x1      Horizontal (x) position for terminating point B.
 * @param[in] color   Color of line to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw line @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_line(int16_t y0, int16_t x0, int16_t y1, int16_t x1,
                      uint16_t color, uint16_t layer)
{
  int16_t steep = (abs(y1 - y0) > abs(x1 - x0));
  int16_t dx = 0, dy = 0;
  int16_t err = 0;
  int16_t ystep = 0;

  if (steep) {
    SSD1306_SWAP(x0, y0);
    SSD1306_SWAP(x1, y1);
  }

  if (x0 > x1) {
    SSD1306_SWAP(x0, x1);
    SSD1306_SWAP(y0, y1);
  }

  dx = x1 - x0;
  dy = abs(y1 - y0);
  err = dx / 2;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      ssd1306_draw_pixel(x0, y0, color, layer);
    } else {
      ssd1306_draw_pixel(y0, x0, color, layer);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

/**
 * @brief   Draws a vertical line.
 * @details Draws a vertical line from a beginning coordinate to certain length.
 *
 * @param[in] y       Vertical (y) coordinate to draw.
 * @param[in] x       Horizontal (x) coordinate to draw.
 * @param[in] h       length of line to draw.
 * @param[in] color   Color of line to draw  @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw line @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_fast_vline(int16_t y, int16_t x, int16_t h,
                             uint16_t color, uint16_t layer)
{
  ssd1306_draw_line(y, x, y+h-1, x, color, layer);
}

/**
 * @brief   Draws a vertical line.
 * @details Draws a vertical line from a beginning coordinate to certain length.
 *
 * @param[in] y       (y) coordinate to draw.
 * @param[in] x       (x) coordinate to draw.
 * @param[in] w       length of line to draw.
 * @param[in] color   Color of line to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw line @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_fast_hline(int16_t y, int16_t x, int16_t w,
                             uint16_t color, uint16_t layer)
{
  ssd1306_draw_line(y, x, y, x+w-1, color, layer);
}

/**
 * @brief   Draws a polygon.
 * @details Draws a polygon from n number of points.
 *
 * @param[in] poly    SSD1306 polygon parameter @ref SSD1306_POLYGON_SPEC.
 * @param[in] y       (y) coordinate to draw.
 * @param[in] x       (x) coordinate to draw.
 * @param[in] color   Color of polygon to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw polygon @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_polygon(ssd1306_poly_t* poly,
                         int16_t y, int16_t x,
                         uint16_t color, uint16_t layer)
{
  int16_t i = 0;

  if (poly->corners < 2) {
    return;
  }

  for (i=0; i<(poly->corners - 1); i++) {
    ssd1306_draw_line(round(poly->point[i].y + y),
                     round(poly->point[i].x + x),
                     round(poly->point[i+1].y + y),
                     round(poly->point[i+1].x + x),
                     color, layer);
  }
  ssd1306_draw_line(round(poly->point[i].y + y),
                   round(poly->point[i].x + x),
                   round(poly->point[0].y + y),
                   round(poly->point[0].x + x),
                   color, layer);
}

/**
 * @brief   Draws a filled polygon.
 * @details http://alienryderflex.com/polygon_fill/.
 *
 * @param[in] poly    SSD1306 polygon parameter @ref SSD1306_POLYGON_SPEC.
 * @param[in] y       (y) coordinate to draw.
 * @param[in] x       (x) coordinate to draw.
 * @param[in] color   Color of polygon to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw polygon @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_fill_polygon(ssd1306_poly_t* poly, double y, double x,
                         uint16_t color, uint16_t layer)
{
  int     nodeX[SSD1306_MAX_POLY_CORNERS], nodes, i, j, swap;
  double  polyX[SSD1306_MAX_POLY_CORNERS], pixelX;
  double  polyY[SSD1306_MAX_POLY_CORNERS], pixelY;

  /* draw shape */
  ssd1306_draw_polygon(poly, (int16_t)y, (int16_t)x, color, layer);
  /* copy polygon */
  i = poly->corners - 1;
  while (i >= 0) {
    polyX[i] = round(poly->point[i].x + x);
    polyY[i] = round(poly->point[i].y + y);
    i--;
  }
  /* Loop through the rows of the image */
  for (pixelY=0; pixelY<SSD1306_DISPLAY_HEIGHT; pixelY++) {
    /* Build a list of nodes */
    nodes = 0;
    j = poly->corners - 1;
    for (i=0; i<poly->corners; i++) {
      if ((polyY[i]<(double) pixelY && polyY[j]>=(double) pixelY) ||
          (polyY[j]<(double) pixelY && polyY[i]>=(double) pixelY)) {
        nodeX[nodes++] = (int)round(  polyX[i] +
                                      (pixelY-polyY[i])/(polyY[j]-polyY[i]) *
                                      (polyX[j]-polyX[i]) );
      }
      j = i;
    }

    /* Sort the nodes, via a simple Bubble sort */
    i = 0;
    while (i<nodes-1) {
      if (nodeX[i]>nodeX[i+1]) {
        swap = nodeX[i];
        nodeX[i] = nodeX[i+1];
        nodeX[i+1] = swap;
        if (i) {
          i--;
        }
      } else {
        i++;
      }
    }

    /* Fill the pixels between node pairs. */
    for (i=0; i<nodes; i+=2) {
      if (nodeX[i]>=SSD1306_DISPLAY_WIDTH) {
        break;
      }
      if (nodeX[i+1]> 0) {
        if (nodeX[i]< 0) {
          nodeX[i] = 0 ;
        }
        if (nodeX[i+1] > SSD1306_DISPLAY_WIDTH) {
          nodeX[i+1] = SSD1306_DISPLAY_WIDTH;
        }
        for (pixelX=nodeX[i]; pixelX<=nodeX[i+1]; pixelX++) {
          ssd1306_draw_pixel((int16_t)(pixelY), (int16_t)(pixelX), color, layer);
        }
      }
    }
  }
}

/**
 * @brief   Fill the complete screen.
 * @details Draws a rectangle to fills complete screen.
 *
 * @param[in] color   Color to fill on screen @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to be filled @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_fill_screen(uint16_t color, uint16_t layer)
{
  ssd1306_fill_rectangle( 0, 0, SSD1306_DISPLAY_HEIGHT, SSD1306_DISPLAY_WIDTH,
                          color, layer);
}

/**
 * @brief   Draws a circle.
 * @details Draws a vertical line from a beginning coordinate to certain length.
 *
 * @param[in] y0      (y) coordinate of circle's center.
 * @param[in] x0      (x) coordinate of circle's center.
 * @param[in] r       radius of circle to draw.
 * @param[in] color   Color of circle to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw circle @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_circle( uint8_t y0, uint8_t x0, uint8_t r,
                          uint16_t color, uint16_t layer)
{
  int x = r;
  int y = 0;
  /* Decision criterion divided by 2 evaluated at x=r, y=0 */
  int check_point = 1 - x;

  if (x < 0) {
    return;
  }

  while (y <= x) {
    ssd1306_draw_pixel( y + y0,  x + x0, color, layer); /* Octant 1 */
    ssd1306_draw_pixel( x + y0,  y + x0, color, layer); /* Octant 2 */
    ssd1306_draw_pixel( y + y0, -x + x0, color, layer); /* Octant 4 */
    ssd1306_draw_pixel( x + y0, -y + x0, color, layer); /* Octant 3 */
    ssd1306_draw_pixel(-y + y0, -x + x0, color, layer); /* Octant 5 */
    ssd1306_draw_pixel(-x + y0, -y + x0, color, layer); /* Octant 6 */
    ssd1306_draw_pixel(-y + y0,  x + x0, color, layer); /* Octant 8 */
    ssd1306_draw_pixel(-x + y0,  y + x0, color, layer); /* Octant 7 */
    y++;

    if (check_point <= 0) {
      /* Change in decision criterion for y -> y+1 */
      check_point += 2 * y + 1;
    } else {
      x--;
      /* Change for y -> y+1, x -> x-1 */
      check_point += 2 * (y - x) + 1;
    }
  }
}

/**
 * @brief   Draws a circle helper.
 * @details Draws a circle helper name.
 *
 * @param[in] y0      (y) coordinate of circle's center.
 * @param[in] x0      (x) coordinate of circle's center.
 * @param[in] r       Radius of circle to draw.
 * @param[in] corner  corner name to display.
 * @param[in] color   Color of circle helper to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_circle_helper(int16_t y0, int16_t x0, int16_t r,
                                uint8_t corner, uint16_t color, uint16_t layer)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corner & 0x8) {
      ssd1306_draw_pixel(y0 + x, x0 - y, color, layer);
      ssd1306_draw_pixel(y0 + y, x0 - x, color, layer);
    }

    if (corner & 0x4) {
      ssd1306_draw_pixel(y0 + y, x0 + x, color, layer);
      ssd1306_draw_pixel(y0 + x, x0 + y, color, layer);
    }

    if (corner & 0x2) {
      ssd1306_draw_pixel(y0 - y, x0 + x, color, layer);
      ssd1306_draw_pixel(y0 - x, x0 + y, color, layer);
    }

    if (corner & 0x1) {
      ssd1306_draw_pixel(y0 - x, x0 - y, color, layer);
      ssd1306_draw_pixel(y0 - y, x0 - x, color, layer);
    }
  }
}

/**
 * @brief   Draws a filled circle.
 * @details Fills circle with defined color.
 *
 * @param[in] y0      (y) coordinate of circle's center.
 * @param[in] x0      (x) coordinate of circle's center.
 * @param[in] r       Radius of circle to draw.
 * @param[in] color   Color of circle to fill @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to fill circle @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_fill_circle( int16_t y0, int16_t x0, int16_t r,
                          uint16_t color, uint16_t layer)
{
  ssd1306_draw_fast_vline(y0-r, x0, 2*r+1, color, layer);
  ssd1306_fill_circle_helper(y0, x0, r, 3, 0, color, layer);
}

/**
 * @brief   Draws a filled the circle helper.
 * @details Used to do circles and round rectangles.
 *
 * @param[in] y0      (y) coordinate of circle's center.
 * @param[in] x0      (x) coordinate of circle's center.
 * @param[in] r       Radius of circle to draw.
 * @param[in] corner  Corner name to display.
 * @param[in] delta   delta for rounding.
 * @param[in] color   Color of circle to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to fill @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_fill_circle_helper(int16_t y0, int16_t x0, int16_t r,
                                uint8_t corner, int16_t delta,
                                uint16_t color, uint16_t layer)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corner & 0x1 ) {
      ssd1306_draw_fast_vline(y0-y, x0+x, 2*y+1+delta, color, layer);
      ssd1306_draw_fast_vline(y0-x, x0+y, 2*x+1+delta, color, layer);
    }
    if (corner & 0x2 ) {
      ssd1306_draw_fast_vline(y0-y, x0-x, 2*y+1+delta, color, layer);
      ssd1306_draw_fast_vline(y0-x, x0-y, 2*x+1+delta, color, layer);
    }
  }
}

/**
 * @brief   Draws a rectangle.
 * @details Draws a rectangle with beginning coordinates and provided height
 *          and breadth.
 *
 * @param[in] y       (y) coordinate of rectangle beginning.
 * @param[in] x       (x) coordinate of rectangle beginning.
 * @param[in] h       Height of rectangle.
 * @param[in] w       Width of rectangle.
 * @param[in] color   Color of rectangle to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw rectangle @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_rectangle(int16_t y, int16_t x, int16_t h, int16_t w,
                            uint16_t color, uint16_t layer)
{
  int16_t x1, y1;
  if ((w == 0) | (h == 0)) {
    return;
  }
  x1 = x + w - 1;
  y1 = y + h - 1;

  if ((h > 2 ) | (w > 2)) {
    ssd1306_draw_line(y, x, y, x1, color, layer);
    ssd1306_draw_line(y1, x, y1, x1, color, layer);
    ssd1306_draw_line(y+1, x, y1-1, x, color, layer);
    ssd1306_draw_line(y+1, x1, y1-1, x1, color, layer);
  } else {
    ssd1306_draw_line(y,  x, y, x1, color, layer);
    ssd1306_draw_line(y1, x, y1, x1, color, layer);
  }
}

/**
 * @brief   Draws a filled rectangle.
 * @details Used to do circles and round rectangles.
 *
 * @param[in] y       (y) coordinate of rectangle beginning.
 * @param[in] x       (x) coordinate of rectangle beginning.
 * @param[in] h       Height of rectangle.
 * @param[in] w       Width of rectangle.
 * @param[in] color   Color of rectangle to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_fill_rectangle(uint8_t y, uint8_t x, uint8_t h, uint8_t w,
                            uint16_t color, uint16_t layer)
{
  uint8_t x0, x1, y1;

  x0 = x;
  x1 = x + w;
  y1 = y + h;
  for (; y < y1; y++) {
    for (x = x0; x < x1; x++) {
      ssd1306_draw_pixel(y, x, color, layer);
    }
  }
}

/**
 * @brief   Draws a bitmap on the screen.
 * @details Used to draw bitmap image of defined size.
 *
 * @param[in] y       (y) coordinate of bitmap beginning.
 * @param[in] x       (x) coordinate of bitmap beginning.
 * @param[in] bitmap  Bitmap image for displaying.
 * @param[in] h       Height of bitmap image.
 * @param[in] w       Width of bitmap image.
 * @param[in] color   Color of bitmap to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw bitmap @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_bitmap( int16_t y, int16_t x, uint8_t *bitmap,
                          int16_t h, int16_t w, uint16_t color, uint16_t layer)
{
  uint8_t *pbitmap;
  int16_t i, j, byteWidth = (w + 7) / 8;

  for (j=0; j<h; j++) {
    for (i=0; i<w; i++ ) {
      pbitmap = bitmap + j * byteWidth + i / 8;
      if ((uint8_t)(*pbitmap) & (128 >> (i & 7))) {
        ssd1306_draw_pixel(y+j, x+i, color, layer);
      }
    }
  }
}

/**
 * @brief   Writes a character on the screen.
 * @details By Default 8514oem character set is used to display.
 *
 * @param[in] y       (y) coordinate to write.
 * @param[in] x       (x) coordinate to write.
 * @param[in] c       Character to display.
 * @param[in] size    Character size to display @ref SSD1306_TEXT_SPEC.
 * @param[in] color   Color of char to write @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to draw bitmap @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void ssd1306_draw_char(int16_t y, int16_t x, uint16_t c, uint8_t size,
                       uint16_t color, uint16_t layer)
{
  int8_t i = 0;
  int8_t j = 0;
  if ((x >= SSD1306_DISPLAY_WIDTH)  ||    /* clip right */
      (y >= SSD1306_DISPLAY_HEIGHT) ||    /* clip bottom */
      ((x + 6 * size - 1) < 0)      ||    /* clip left */
      ((y + 8 * size - 1) < 0)) {         /* clip top */
    return;
  }

  for (i=0; i<6; i++) {
    int8_t line;
    if (i > SSD1306_TEXT_MAX_LINE) {
      line = 0x0;
    } else {
      line = (int8_t)*(i2_font5x7+(c*5)+i);
    }

    for (j = 0; j<8; j++) {
      if (color == SSD1306_WHITE) {
        if (line & 0x1) {
          if (size == 1) {    /* default size */
            ssd1306_draw_pixel(y+j, x+i, color, layer);
          } else {            /* big size */
            ssd1306_draw_rectangle( y+(j*size), x+(i*size), size,
                                    size, color, layer);
          }
        }
      }
      if (color == SSD1306_INVERSE) {
        if (!(line & 0x1)) {
          if (size == 1) {    /* default size */
            ssd1306_draw_pixel(y+j, x+i, color, layer);
          } else {            /* big size */
            ssd1306_draw_rectangle( y+(j*size), x+(i*size), size,
                                    size, color, layer);
          }
        }
      }
      line >>= 1;
    }
  }
  ssd1306_refresh();
}

/**
 * @brief   Writes a character on the screen.
 * @details By Default 8514oem character set is used to display.
 *
 * @param[in] y       (y) coordinate to write.
 * @param[in] x       (x) coordinate to write.
 * @param[in] c       Character to display.
 * @param[in] size    Character size to display @ref SSD1306_TEXT_SPEC.
 * @param[in] color   Color of char to write @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to write @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void  ssd1306_draw_char_loc(int16_t y, int16_t x, uint16_t c, uint8_t size,
                            uint16_t color, uint16_t layer)
{
  ssd1306_clear_char(y, x, size);
  if (size == SSD1306_TEXT_SIZE_LARGE) {
    x *= 6;
    y *= 10;
  } else if (size == SSD1306_TEXT_SIZE_EXTREA_LARGE) {
    x *= 12;
    y *= 20;
  }
  ssd1306_draw_char(y, x, c, size, color, layer);
}

/**
 * @brief   Writes a string on the screen.
 * @details By Default 8514oem character set is used to display.
 *
 * @param[in] y       (y) coordinate to write.
 * @param[in] x       (x) coordinate to write.
 * @param[in] text    Character to display.
 * @param[in] size    Character size to display @ref SSD1306_TEXT_SPEC.
 * @param[in] color   Color of char to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to write string @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void  ssd1306_draw_string(int16_t y, int16_t x, char *text, uint8_t size,
                          uint16_t color, uint16_t layer)
{
  uint8_t l = 0;
  while (*text) {
    ssd1306_draw_char(y, x + (l * (5*size + 1)), *text, size, color, layer);
    text++;
    l++;
  }
}

/**
 * @brief   Writes a string on the screen at specified location.
 * @details By Default 8514oem character set is used to display.
 *
 * @param[in] y       (y) coordinate to write.
 * @param[in] x       (x) coordinate to write.
 * @param[in] text    Character to display.
 * @param[in] size    Character size to display @ref SSD1306_TEXT_SPEC.
 * @param[in] color   Color of char to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to write string @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void  ssd1306_draw_string_loc(int16_t y, int16_t x, char *text, uint8_t size,
                              uint16_t color, uint16_t layer)
{
  while (*text) {
    ssd1306_draw_char_loc(y, x, *text, size, color, layer);
    text++;
    x++;
  }
}

/**
 * @brief   Types a string on the screen at specified location.
 * @details By Default 8514oem character set is used to display.
 *
 * @param[in] y       (y) coordinate to write.
 * @param[in] x       (x) coordinate to write.
 * @param[in] text    Character to display.
 * @param[in] size    Character size to display @ref SSD1306_TEXT_SPEC.
 * @param[in] delay   Delay between writing two characters.
 * @param[in] color   Color of char to draw @ref SSD1306_COLOR_SPEC.
 * @param[in] layer   Layer to write string @ref SSD1306_MULTI_LAYER_SPEC.
 * @return  None.
 */
void  ssd1306_type_string_loc(int16_t y, int16_t x, char *text, uint8_t size,
                              uint8_t delay, uint16_t color, uint16_t layer)
{
  uint8_t i;
  while (*text) {
    ssd1306_draw_char_loc(y, x, *text, size, color, layer);
    text++;
    x++;

    i = 0;
    while (i < delay) {
      ssd1306_delay(1);
      i++;
    }
    ssd1306_refresh();
  }
}

/**
 * @brief   Shifts content of frame buffer up the specified number of pixels.
 * @details Layer 1 will have shifting options and a blank space will be added
 *          at bottom after shifting.
 *
 * @param[in] height    Number of pixels to shift the frame buffer up.
 * @param[in] direction Direction to shift in.
 * @return  None.
 */
void ssd1306_shift_frame_buffer(uint16_t height, uint16_t direction)
{
  int16_t y = 0;
  int16_t x = 0;
  if (height == 0) {
    return;
  }

#if 0
  while (height > 0)
  {
#endif /* 0 */
    switch (direction) {
    case 0 :
      /* This is horribly inefficient, but at least easy to understand. */
      /* In a production environment, this should be significantly optimized */
      if (height >= SSD1306_DISPLAY_HEIGHT) {
        /* Clear the entire frame buffer */
        ssd1306_clear_screen(SSD1306_LAYER1);
        return;
      }
      for (y=0; y<SSD1306_DISPLAY_HEIGHT; y++) {
        for (x=0; x<SSD1306_DISPLAY_WIDTH; x++) {
          if ((SSD1306_DISPLAY_HEIGHT ) - y > 1) {
            /* Shift height from further ahead in the buffer */
            ssd1306_get_pixel(y + 1, x) ?  \
                ssd1306_draw_pixel(y, x, SSD1306_WHITE, SSD1306_LAYER1) : \
                ssd1306_draw_pixel(y, x, SSD1306_BLACK, SSD1306_LAYER1);
          } else {
            /* Clear the entire line */
            ssd1306_draw_pixel(y, x, SSD1306_BLACK, SSD1306_LAYER1);
          }
        }
      }
      break;
    case 1 :
      y = SSD1306_COUNTOF(ssd1306_buffer_layer1);
      x = y;
      for (x=SSD1306_COUNTOF(ssd1306_buffer_layer1)-1; x>=0; x--) {
        ssd1306_buffer_layer1[x] = ssd1306_buffer_layer1[x] << height;
        if (((x - (int16_t)SSD1306_COUNTOF(ssd1306_buffer_layer1)/8) >= 0) &&
            ((ssd1306_buffer_layer1[x - SSD1306_COUNTOF(ssd1306_buffer_layer1)/8] & 0x80) == 0x80)) {
          ssd1306_buffer_layer1[x] = ssd1306_buffer_layer1[x] | 0x01;
        }
      }
      break;
    case 2 :
      for (y=0; y<=7; y++) {
        for (x=0; x<(SSD1306_COUNTOF(ssd1306_buffer_layer1)/8 - 1); x++) {
          ssd1306_buffer_layer1[x + (128 * y)] = ssd1306_buffer_layer1[x + (128 * y) + 1];
        }
        ssd1306_buffer_layer1[127 + (128 * y)] = 0;
      }
      break;
    case 3 :
      for (y=0; y<=7; y++) {
        for (x=(SSD1306_COUNTOF(ssd1306_buffer_layer1)/8 - 1); x>0; x--) {
          ssd1306_buffer_layer1[x + (128 * y)] = ssd1306_buffer_layer1[x - 1 + (128 * y)];
        }
        ssd1306_buffer_layer1[128 * y] = 0;
      }
      break;
    default:
      return;
    }
#if 0
  height--;
  }
#endif /* 0 */
}

/**
 * @brief   Inserts time delay.
 * @details Need to handle blocking and non blocking both delays.
 *
 * @param[in] nCount    Delay count in ms.
 * @return  None.
 */
void ssd1306_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for (index = (100000 * nCount); index != 0; index--) {
  }
}

#if ( SSD1306_MULTILAYER_SUPPORT == I2_ENABLE )
/**
 * @brief   Mixes both layers together.
 * @details Mixes both layers buffer together.
 *
 * @return  None.
 */
void ssd1306_mix_frame_buffer(void)
{
  uint16_t i;
  for (i=0; i<(SSD1306_DISPLAY_WIDTH * SSD1306_DISPLAY_HEIGHT / 8); i++) {
    ssd1306_buffer_layer2[i] |= ssd1306_buffer_layer1[i];
  }
}
#endif /* SSD1306_MULTILAYER_SUPPORT */

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
