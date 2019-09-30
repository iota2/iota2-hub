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

/* Includes ------------------------------------------------------------------*/
#include "i2_stm32f4xx_hal_uart.h"
#include "i2_stm32f4xx_hal_gpio.h"
#include "i2_stm32f4xx_hal_common.h"

#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_gpio_ex.h"

#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif /* ENABLE_RTOS_AWARE_HAL */

/* Private defines -----------------------------------------------------------*/
#define I2_UART_FIFO_HALF_SIZE            ( I2_UART_FIFO_SIZE / 2 )

/* Definition for UART Priority */
#define UART_PREEMPTION_PRIORITY          ( 5 )
#define UART_SUB_PRIORITY                 ( 1 )
#define UART_DMA_PREEMPTION_PRIORITY      ( 5 )
#define UART_DMA_SUB_PRIORITY             ( 1 )

/* iota2 UART CONFIGURATION ****************************************************
 *
  +=========+=======+=========+=====+=======+===========+===========+
  | UART    | PIN   | GPIO    | AF  | DMA   | CHANNEL   | STREAM    |
  +=========+=======+=========+=====+=======+===========+===========+
  | USART1  | RX    | PA10    | AF7 | DMA2  | CHANNEL4  | STREAM2   |
  |         |       | PB7     |     |       |           | STREAM5   |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PA9     |     |       |           | STREAM7   |
  |         |       | PB9     |     |       |           |           |
  +=========+=======+=========+=====+=======+===========+===========+
  | USART2  | RX    | PA3     | AF7 | DMA1  | CHANNEL4  | STREAM5   |
  |         |       | PD6     |     |       |           |           |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PA2     |     |       |           | STREAM6   |
  |         |       | PD5     |     |       |           |           |
  +=========+=======+=========+=====+=======+===========+===========+
  | USART3  | RX    | PB11    | AF7 | DMA1  | CHANNEL4  | STREAM1   |
  |         |       | PC11    |     |       |           |           |
  |         |       | PD9     |     |       |           |           |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PB10    |     |       |           | STREAM3   |
  |         |       | PC10    |     |       |           |           |
  |         |       | PD8     |     |       |           |           |
  +=========+=======+=========+=====+=======+===========+===========+
  | UART4   | RX    | PA1     | AF8 | DMA1  | CHANNEL4  | STREAM2   |
  |         |       | PC11    |     |       |           |           |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PA0     |     |       |           | STREAM4   |
  |         |       | PC10    |     |       |           |           |
  +=========+=======+=========+=====+=======+===========+===========+
  | UART5   | RX    | PC12    | AF8 | DMA1  | CHANNEL4  | STREAM0   |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PA0     |     |       |           | STREAM7   |
  +=========+=======+=========+=====+=======+===========+===========+
  | USART6  | RX    | PC7     | AF8 | DMA2  | CHANNEL5  | STREAM1   |
  |         |       | PG9     |     |       |           | STREAM2   |
  |         +-------+---------+     |       |           +-----------+
  |         | TX    | PC6     |     |       |           | STREAM6   |
  |         |       | PG14    |     |       |           | STREAM7   |
  +=========+=======+=========+=====+=======+===========+===========+

*******************************************************************************/

/* Defined Number of UART Modules to Enable ***********************************/
#define I2_MAX_NUM_UART_CONTEXT           ( 6 )
#define I2_ENABLE_UART1_CONTEXT
#define I2_ENABLE_UART2_CONTEXT
#define I2_ENABLE_UART3_CONTEXT
#define I2_ENABLE_UART4_CONTEXT
#define I2_ENABLE_UART5_CONTEXT
#define I2_ENABLE_UART6_CONTEXT

/* UART1 CONFIGURATION SELECTION **********************************************/
/* Definition for UART1 Pins, to be defined as < PORT, PIN > */
#define UART1_RX_GPIO_CONFIG              GPIOA, GPIO_PIN_9
#define UART1_TX_GPIO_CONFIG              GPIOA, GPIO_PIN_10
/* Definition for UART1 DMA, to be defined as < CHANNEL, STREAM > */
#define UART1_DMA_ENABLE                  I2_DISABLE
#define UART1_RX_DMA_CONFIG               DMA_CHANNEL_4, DMA2_Stream5
#define UART1_TX_DMA_CONFIG               DMA_CHANNEL_4, DMA2_Stream7
/* Definition for UART1 NVIC */
#define UART1_DMA_RX_IRQn                 DMA2_Stream5_IRQn
#define UART1_DMA_TX_IRQn                 DMA2_Stream7_IRQn
#define UART1_DMA_RX_IRQHandler           DMA2_Stream5_IRQHandler
#define UART1_DMA_TX_IRQHandler           DMA2_Stream7_IRQHandler
/******************************************************************************/

/* UART2 CONFIGURATION SELECTION **********************************************/
/* Definition for UART2 Pins, to be defined as < PORT, PIN > */
#define UART2_RX_GPIO_CONFIG              GPIOA, GPIO_PIN_2
#define UART2_TX_GPIO_CONFIG              GPIOA, GPIO_PIN_3
/* Definition for UART2 DMA, to be defined as < CHANNEL, STREAM > */
#define UART2_DMA_ENABLE                  I2_DISABLE
#define UART2_RX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream5
#define UART2_TX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream6
/* Definition for UART2 NVIC */
#define UART2_DMA_RX_IRQn                 DMA1_Stream5_IRQn
#define UART2_DMA_TX_IRQn                 DMA1_Stream6_IRQn
#define UART2_DMA_RX_IRQHandler           DMA1_Stream5_IRQHandler
#define UART2_DMA_TX_IRQHandler           DMA1_Stream6_IRQHandler
/******************************************************************************/

/* UART3 CONFIGURATION SELECTION **********************************************/
/* Definition for UART3 Pins, to be defined as < PORT, PIN > */
#define UART3_RX_GPIO_CONFIG              GPIOB, GPIO_PIN_10
#define UART3_TX_GPIO_CONFIG              GPIOB, GPIO_PIN_11
/* Definition for UART3 DMA, to be defined as < CHANNEL, STREAM > */
#define UART3_DMA_ENABLE                  I2_DISABLE
#define UART3_RX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream1
#define UART3_TX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream3
/* Definition for UART3 NVIC */
#define UART3_DMA_RX_IRQn                 DMA1_Stream1_IRQn
#define UART3_DMA_TX_IRQn                 DMA1_Stream3_IRQn
#define UART3_DMA_RX_IRQHandler           DMA1_Stream1_IRQHandler
#define UART3_DMA_TX_IRQHandler           DMA1_Stream3_IRQHandler
/******************************************************************************/

/* UART4 CONFIGURATION SELECTION **********************************************/
/* Definition for UART4 Pins, to be defined as < PORT, PIN > */
#define UART4_RX_GPIO_CONFIG              GPIOC, GPIO_PIN_10
#define UART4_TX_GPIO_CONFIG              GPIOC, GPIO_PIN_11
/* Definition for UART4 DMA, to be defined as < CHANNEL, STREAM > */
#define UART4_DMA_ENABLE                  I2_DISABLE
#define UART4_RX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream2
#define UART4_TX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream4
/* Definition for UART4 NVIC */
#define UART4_DMA_RX_IRQn                 DMA1_Stream2_IRQn
#define UART4_DMA_TX_IRQn                 DMA1_Stream4_IRQn
#define UART4_DMA_RX_IRQHandler           DMA1_Stream2_IRQHandler
#define UART4_DMA_TX_IRQHandler           DMA1_Stream4_IRQHandler
/******************************************************************************/

/* UART5 CONFIGURATION SELECTION **********************************************/
/* Definition for UART5 Pins, to be defined as < PORT, PIN > */
#define UART5_RX_GPIO_CONFIG              GPIOC, GPIO_PIN_10
#define UART5_TX_GPIO_CONFIG              GPIOD, GPIO_PIN_2
/* Definition for UART5 DMA, to be defined as < CHANNEL, STREAM > */
#define UART5_DMA_ENABLE                  I2_DISABLE
#define UART5_RX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream0
#define UART5_TX_DMA_CONFIG               DMA_CHANNEL_4, DMA1_Stream7
/* Definition for UART5 NVIC */
#define UART5_DMA_RX_IRQn                 DMA1_Stream0_IRQn
#define UART5_DMA_TX_IRQn                 DMA1_Stream7_IRQn
#define UART5_DMA_RX_IRQHandler           DMA1_Stream0_IRQHandler
#define UART5_DMA_TX_IRQHandler           DMA1_Stream7_IRQHandler
/******************************************************************************/

/* UART6 CONFIGURATION SELECTION **********************************************/
/* Definition for UART6 Pins, to be defined as < PORT, PIN > */
#define UART6_RX_GPIO_CONFIG              GPIOC, GPIO_PIN_6
#define UART6_TX_GPIO_CONFIG              GPIOC, GPIO_PIN_7
/* Definition for UART6 DMA, to be defined as < CHANNEL, STREAM > */
#define UART6_DMA_ENABLE                  I2_DISABLE
#define UART6_RX_DMA_CONFIG               DMA_CHANNEL_5, DMA2_Stream1
#define UART6_TX_DMA_CONFIG               DMA_CHANNEL_5, DMA2_Stream2
/* Definition for UART6 NVIC */
#define UART6_DMA_RX_IRQn                 DMA2_Stream1_IRQn
#define UART6_DMA_TX_IRQn                 DMA2_Stream2_IRQn
#define UART6_DMA_RX_IRQHandler           DMA2_Stream1_IRQHandler
#define UART6_DMA_TX_IRQHandler           DMA2_Stream2_IRQHandler
/******************************************************************************/

typedef struct {
  const char*           ctx_name;
  i2_gpio_inst_t        rx;
  i2_gpio_inst_t        tx;
  i2_gpio_inst_t        cts;
  i2_gpio_inst_t        rts;
  int32_t               baud_rate;
  int32_t               rx_hal_mode;
  int32_t               tx_hal_mode;
  bool                  initialized;
  USART_TypeDef*        baseaddr;
  uint32_t              dma_rx_channel;
  DMA_Stream_TypeDef*   dma_rx_stream;
  uint32_t              dma_rx_priority;
  IRQn_Type             dma_rx_irqn;
  uint32_t              dma_tx_channel;
  DMA_Stream_TypeDef*   dma_tx_stream;
  uint32_t              dma_tx_priority;
  IRQn_Type             dma_tx_irqn;
  UART_HandleTypeDef    uart;
  DMA_HandleTypeDef     hdma_rx;
  DMA_HandleTypeDef     hdma_tx;
#if defined ( ENABLE_RTOS_AWARE_HAL )
  SemaphoreHandle_t     mutex_rx;
  SemaphoreHandle_t     mutex_tx;
  SemaphoreHandle_t     sem_rx;
  SemaphoreHandle_t     sem_tx;
#endif /* ENABLE_RTOS_AWARE_HAL */
  __IO ITStatus         tx_status;
  __IO ITStatus         rx_status;
  i2_fifo_t             rx_fifo;
  uint8_t               buff[I2_UART_FIFO_SIZE];
  int32_t               rx_water_mark;
  int32_t               rx_trigger_level;
  bool                  rx_buffering_on;
} i2_uart_ctx_t;

static i2_uart_ctx_t i2_uart_ctx_table[I2_MAX_NUM_UART_CONTEXT] = {
#if defined  ( I2_ENABLE_UART1_CONTEXT )
  {
    "USART1",
    { "usart1_rx",  UART1_RX_GPIO_CONFIG },
    { "usart1_tx",  UART1_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  USART1,
    UART1_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART1_DMA_RX_IRQn,
    UART1_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART1_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_UART1_CONTEXT */
#if defined ( I2_ENABLE_UART2_CONTEXT )
  {
    "USART2",
    { "usart2_rx",  UART2_RX_GPIO_CONFIG },
    { "usart2_tx",  UART2_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  USART2,
    UART2_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART2_DMA_RX_IRQn,
    UART2_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART2_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_UART2_CONTEXT */
#if defined ( I2_ENABLE_UART3_CONTEXT )
  {
    "USART3",
    { "usart3_rx",  UART3_RX_GPIO_CONFIG },
    { "usart3_tx",  UART3_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  USART3,
    UART3_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART3_DMA_RX_IRQn,
    UART3_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART3_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_UART3_CONTEXT */
#if defined ( I2_ENABLE_UART4_CONTEXT )
  {
    "UART4",
    { "uart4_rx",   UART4_RX_GPIO_CONFIG },
    { "uart4_tx",   UART4_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  UART4,
    UART4_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART4_DMA_RX_IRQn,
    UART4_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART4_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_UART4_CONTEXT */
#if defined ( I2_ENABLE_UART5_CONTEXT )
  {
    "UART5",
    { "uart5_rx",   UART5_RX_GPIO_CONFIG },
    { "uart5_tx",   UART5_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  UART5,
    UART5_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART5_DMA_RX_IRQn,
    UART5_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART5_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_UART5_CONTEXT */
#if defined ( I2_ENABLE_UART6_CONTEXT )
  {
    "UART6",
    { "uart6_rx",   UART6_RX_GPIO_CONFIG },
    { "uart6_tx",   UART6_TX_GPIO_CONFIG },
    { 0, 0, 0 }, { 0, 0, 0 },
    115200,   INTERRUPT_MODE, POLLING_MODE, false,  USART6,
    UART6_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     UART6_DMA_RX_IRQn,
    UART6_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   UART6_DMA_TX_IRQn,
  }
#endif /* I2_ENABLE_UART6_CONTEXT */
};

static i2_uart_inst_t *i2_uart_inst_table[I2_MAX_NUM_UART_CONTEXT];

/* Define UART Contexts */
#if defined ( I2_ENABLE_UART1_CONTEXT )
static UART_HandleTypeDef *usart1   = NULL;
#endif /* I2_ENABLE_UART1_CONTEXT */
#if defined ( I2_ENABLE_UART2_CONTEXT )
static UART_HandleTypeDef *usart2   = NULL;
#endif /* I2_ENABLE_UART2_CONTEXT */
#if defined ( I2_ENABLE_UART3_CONTEXT )
static UART_HandleTypeDef *usart3   = NULL;
#endif /* I2_ENABLE_UART3_CONTEXT */
#if defined ( I2_ENABLE_UART4_CONTEXT )
static UART_HandleTypeDef *uart4    = NULL;
#endif /* I2_ENABLE_UART4_CONTEXT */
#if defined ( I2_ENABLE_UART5_CONTEXT )
static UART_HandleTypeDef *uart5    = NULL;
#endif /* I2_ENABLE_UART5_CONTEXT */
#if defined ( I2_ENABLE_UART6_CONTEXT )
static UART_HandleTypeDef *usart6   = NULL;
#endif /* I2_ENABLE_UART6_CONTEXT */

/* Private functions ---------------------------------------------------------*/
static i2_uart_ctx_t* uart_get_ctx_from_name(const char *name)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_UART_CONTEXT; i++) {
    if (!strcmp(i2_uart_ctx_table[i].ctx_name, name)) {
      return &i2_uart_ctx_table[i];
    }
  }
  return NULL;
}

static i2_uart_ctx_t* uart_get_ctx_from_handle(UART_HandleTypeDef *huart)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_UART_CONTEXT; i++) {
    if (&i2_uart_ctx_table[i].uart == huart) {
      return &i2_uart_ctx_table[i];
    }
  }
  return NULL;
}

/* Public functions ----------------------------------------------------------*/
i2_error i2_uart_init(i2_uart_inst_t *inst)
{
  i2_error retval = I2_SUCCESS;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;
  USART_TypeDef *base;
  IRQn_Type irqn;

  uint32_t alt;
  uint32_t hwcontrol = UART_HWCONTROL_NONE;
  uint32_t mode = 0;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  /* get UART context for required name */
  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* Prevent re-init */
  if ( ctx->initialized ) {
    return I2_NOT_AVAILABLE;
  }

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  /* Get the UART controller base address */
  base = ctx->baseaddr;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Create mutexes. */
  ctx->mutex_tx = xSemaphoreCreateMutex();
  ctx->mutex_rx = xSemaphoreCreateMutex();

  /* Create semaphores */
  ctx->sem_tx = xSemaphoreCreateBinary();
  ctx->sem_rx = xSemaphoreCreateBinary();
#endif /* ENABLE_RTOS_AWARE_HAL */
  ctx->tx_status = I2_TRANSFER_DONE;
  ctx->rx_status = I2_TRANSFER_DONE;

  if ( base == USART1 ) {
#if defined ( I2_ENABLE_UART1_CONTEXT )
    usart1 = huart;
    irqn = USART1_IRQn;
    alt = GPIO_AF7_USART1;            /* USART1 will always associate to AF7 */
    __HAL_RCC_USART1_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA2_CLK_ENABLE();    /* USART1 will always associate to DMA2 */
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART1_CONTEXT */
  } else if ( base == USART2 ) {
#if defined ( I2_ENABLE_UART2_CONTEXT )
    irqn = USART2_IRQn;
    usart2 = huart;
    alt = GPIO_AF7_USART2;            /* USART2 will always associate to AF7 */
    __HAL_RCC_USART2_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART2_CONTEXT */
  } else if ( base == USART3 ) {
#if defined ( I2_ENABLE_UART3_CONTEXT )
    usart3 = huart;
    irqn = USART3_IRQn;
    alt = GPIO_AF7_USART3;            /* USART3 will always associate to AF7 */
    __HAL_RCC_USART3_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART3_CONTEXT */
  } else if ( base == UART4 ) {
#if defined ( I2_ENABLE_UART4_CONTEXT )
    uart4 = huart;
    irqn = UART4_IRQn;
    alt = GPIO_AF8_UART4;            /* UART4 will always associate to AF8 */
    __HAL_RCC_UART4_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART4_CONTEXT */
  } else if ( base == UART5 ) {
#if defined ( I2_ENABLE_UART5_CONTEXT )
    uart5 = huart;
    irqn = UART5_IRQn;
    alt = GPIO_AF8_UART5;            /* UART5 will always associate to AF8 */
    __HAL_RCC_UART5_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART5_CONTEXT */
  } else if ( base == USART6 ) {
#if defined ( I2_ENABLE_UART6_CONTEXT )
    usart6 = huart;
    irqn = USART6_IRQn;
    alt = GPIO_AF8_USART6;            /* USART6 will always associate to AF8 */
    __HAL_RCC_USART6_CLK_ENABLE();
    if ( (ctx->rx_hal_mode == DMA_MODE) ||
         (ctx->tx_hal_mode == DMA_MODE) ) {
      __HAL_RCC_DMA2_CLK_ENABLE();
    }
#else
    return I2_NOT_AVAILABLE;
#endif /* I2_ENABLE_UART6_CONTEXT */
  } else {
    return I2_INVALID_PARAM;
  }

  /* UART driver needs gpio */
  i2_gpio_init();

  /* Configure TX */
  if ( i2_gpio_is_valid(&ctx->tx) ) {
    i2_gpio_config_alt(&ctx->tx, GPIO_MODE_AF_PP, GPIO_PULLUP, alt);
    mode |= UART_MODE_TX;
  }

  /* Configure RX */
  if ( i2_gpio_is_valid(&ctx->rx) ) {
    i2_gpio_config_alt(&ctx->rx, GPIO_MODE_AF_PP, GPIO_PULLUP, alt);
    mode |= UART_MODE_RX;
  }

  /* Configure CTS */
  if ( i2_gpio_is_valid(&ctx->cts) ) {
    i2_gpio_config_alt(&ctx->cts, GPIO_MODE_AF_OD, GPIO_PULLUP, alt);
    hwcontrol |= UART_HWCONTROL_CTS;
  }

  /* Configure RTS */
  if ( i2_gpio_is_valid(&ctx->rts) ) {
    i2_gpio_config_alt(&ctx->rts, GPIO_MODE_AF_OD, GPIO_PULLUP, alt);
    hwcontrol |= UART_HWCONTROL_RTS;
  }

  /* DMA RX Config */
  if (ctx->rx_hal_mode == DMA_MODE) {
    /* Configure the DMA handler for reception process */
    ctx->hdma_rx.Instance                 = ctx->dma_rx_stream;
    ctx->hdma_rx.Init.Channel             = ctx->dma_rx_channel;
    ctx->hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    ctx->hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    ctx->hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    ctx->hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ctx->hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    ctx->hdma_rx.Init.Mode                = DMA_CIRCULAR;
    ctx->hdma_rx.Init.Priority            = ctx->dma_rx_priority;
    ctx->hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    ctx->hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    ctx->hdma_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
    ctx->hdma_rx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

    HAL_DMA_Init(&ctx->hdma_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, ctx->hdma_rx);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(ctx->dma_rx_irqn, UART_DMA_PREEMPTION_PRIORITY,
                          UART_DMA_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ctx->dma_rx_irqn);
  }

  /* DMA TX Config */
  if (ctx->tx_hal_mode == DMA_MODE) {
    /* Configure the DMA handler for Transmission process */
    ctx->hdma_tx.Instance                 = ctx->dma_tx_stream;
    ctx->hdma_tx.Init.Channel             = ctx->dma_tx_channel;
    ctx->hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    ctx->hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    ctx->hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    ctx->hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ctx->hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    ctx->hdma_tx.Init.Mode                = DMA_NORMAL;
    ctx->hdma_tx.Init.Priority            = ctx->dma_tx_priority;
    ctx->hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    ctx->hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    ctx->hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
    ctx->hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

    HAL_DMA_Init(&ctx->hdma_tx);

    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, ctx->hdma_tx);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(ctx->dma_tx_irqn, UART_DMA_PREEMPTION_PRIORITY,
                          UART_DMA_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ctx->dma_tx_irqn);
  }

  if ((ctx->rx_hal_mode == DMA_MODE) || (ctx->rx_hal_mode == INTERRUPT_MODE) ||
      (ctx->tx_hal_mode == DMA_MODE) || (ctx->tx_hal_mode == INTERRUPT_MODE) ) {
    /* NVIC configuration for UART */
    HAL_NVIC_SetPriority(irqn, UART_PREEMPTION_PRIORITY, UART_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(irqn);
  }

  /* UART configuration */
  huart->Instance                      = base;
  huart->Init.BaudRate                 = ctx->baud_rate;
  huart->Init.HwFlowCtl                = hwcontrol;
  huart->Init.Mode                     = mode;
  /* Assume these values can be fixed for all UART for now */
  huart->Init.WordLength               = UART_WORDLENGTH_8B;
  huart->Init.StopBits                 = UART_STOPBITS_1;
  huart->Init.Parity                   = UART_PARITY_NONE;
  huart->Init.OverSampling             = UART_OVERSAMPLING_8;

  if (HAL_UART_Init(huart) != HAL_OK) {
    retval = I2_FAILURE;
  } else {
    int32_t i;
    for ( i = 0; i < I2_MAX_NUM_UART_CONTEXT; i++ ) {
      if ( !i2_uart_inst_table[i] ) {
        i2_uart_inst_table[i] = inst;
        break;
      }
    }
  }

  i2_fifo_init(&ctx->rx_fifo, ctx->buff, I2_UART_FIFO_SIZE);

  ctx->initialized        = true;
  ctx->rx_trigger_level   = I2_UART_FIFO_HALF_SIZE;
  ctx->rx_buffering_on    = false;
  ctx->rx_water_mark      = 0;

  return retval;
}

i2_error i2_uart_reset(i2_uart_inst_t *inst)
{
  i2_error err = I2_SUCCESS;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  retval = HAL_UART_Abort(huart);
  if ( retval != HAL_OK ) {
    goto out;
  }

  huart->Init.BaudRate = ctx->baud_rate;
  retval = HAL_UART_Init(huart);

  i2_fifo_reset(&ctx->rx_fifo);
#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Reset the semaphore back to 0 */
  xSemaphoreTake(ctx->sem_rx, (TickType_t)1);
#endif /* ENABLE_RTOS_AWARE_HAL */
  ctx->tx_status = I2_TRANSFER_DONE;
  ctx->rx_status = I2_TRANSFER_DONE;

out:
  err = i2_get_hal_error( retval );
  return err;
}

i2_uart_inst_t* i2_uart_inst_get(char *inst_name)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_UART_CONTEXT; i++) {
    if (!strcmp(i2_uart_inst_table[i]->inst_name, inst_name)) {
      return i2_uart_inst_table[i];
    }
  }

  return NULL;
}

i2_error i2_uart_tx(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->mutex_tx, (TickType_t)timeout) == pdFALSE) {
    if ( num_bytes ) {
      *num_bytes = 0;
    }
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  ctx->tx_status = I2_TRANSFER_WAIT;

  if (ctx->tx_hal_mode == INTERRUPT_MODE) {
    retval = HAL_UART_Transmit_IT(huart, txbuf, size);
  } else if (ctx->tx_hal_mode == DMA_MODE) {
    retval = HAL_UART_Transmit_DMA(huart, txbuf, size);
  } else {
    err = I2_NOT_SUPPORTED;
    goto err;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->sem_tx, (TickType_t)timeout) == pdFALSE) {
    retval = HAL_TIMEOUT;
  } /* ENABLE_RTOS_AWARE_HAL */
#else
  while (ctx->tx_status == I2_TRANSFER_WAIT);
#endif

  err = i2_get_hal_error(retval);

  /* Check for error interrupt */
  if ( ctx->tx_status == I2_TRANSFER_ERROR ) {
    err = I2_FAILURE;
  } else if (ctx->tx_status == I2_TRANSFER_WAIT) {
  /* Tx Complete interrupt was not received */
    err = I2_TIMEOUT;
  }

  /* Record correct number of sent bytes */
  if ( num_bytes ) {
    if ((err == I2_SUCCESS) || (err == I2_TIMEOUT)) {
      *num_bytes = (size - huart->TxXferCount);
    } else {
      *num_bytes = 0;
    }
  }

  if ( err != I2_SUCCESS ) {
    HAL_UART_AbortTransmit(huart);
  }

err:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex_tx);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return err;
}

i2_error i2_uart_tx_polling(i2_uart_inst_t *inst, uint8_t *txbuf, int32_t size,
                          int32_t *num_bytes, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->mutex_tx, (TickType_t)timeout) == pdFALSE) {
    if ( num_bytes ) {
      *num_bytes = 0;
    }
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  retval = HAL_UART_Transmit(huart, txbuf, size, (TickType_t)timeout);
  err = i2_get_hal_error(retval);

  /* Record correct number of sent bytes */
  if ( num_bytes ) {
    if ((err == I2_SUCCESS) || (err == I2_TIMEOUT)) {
      *num_bytes = (size - huart->TxXferCount);
    } else {
      *num_bytes = 0;
    }
  }

  if ( err != I2_SUCCESS ) {
    HAL_UART_AbortTransmit(huart);
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex_tx);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return err;
}

/*
 * Start buffering continuous RX stream
 */
i2_error i2_uart_rx_buffering_start(i2_uart_inst_t *inst)
{
  i2_error err = I2_SUCCESS;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  if ( ctx->rx_hal_mode == INTERRUPT_MODE ) {
    retval = HAL_UART_Receive_IT(huart, (uint8_t*)ctx->buff, I2_UART_FIFO_SIZE);
  } else if ( ctx->rx_hal_mode == DMA_MODE ) {
    retval = HAL_UART_Receive_DMA(huart, (uint8_t*)ctx->buff, I2_UART_FIFO_SIZE);
  } else {
    err = I2_NOT_SUPPORTED;
  }

  if ( err == I2_SUCCESS ) {
    err = i2_get_hal_error(retval);
    if ( err == I2_SUCCESS ) {
      ctx->rx_buffering_on = true;
    }
  }

  return err;
}

/*
 * Stop buffering continuous rx stream
 */
i2_error i2_uart_rx_buffering_stop(i2_uart_inst_t *inst)
{
  i2_error err = I2_SUCCESS;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  retval = HAL_UART_AbortReceive(huart);
  i2_fifo_reset(&ctx->rx_fifo);

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Reset the semaphore back to 0 */
  xSemaphoreTake(ctx->sem_rx, (TickType_t)1);
#endif /* ENABLE_RTOS_AWARE_HAL */
  ctx->rx_status = I2_TRANSFER_DONE;
  ctx->rx_water_mark = 0;

  err = i2_get_hal_error(retval);
  if (err == I2_SUCCESS) {
    ctx->rx_buffering_on = false;
  }

  return err;
}

i2_error i2_uart_rx(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                  int32_t *num_bytes, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  i2_uart_ctx_t *ctx;
  int32_t _size;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Make sure one thread uses one controller at a time */
  if (xSemaphoreTake(ctx->mutex_rx, (TickType_t)timeout) == pdFALSE) {
    if ( num_bytes ) {
      *num_bytes = 0;
    }
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* desired read size */
  _size = size;

  /* If the FIFO is empty, wait for signal */
  if ( !i2_fifo_count(&ctx->rx_fifo, false) ) {
      ctx->rx_status = I2_TRANSFER_WAIT;
 #if defined ( ENABLE_RTOS_AWARE_HAL)
      if (xSemaphoreTake(ctx->sem_rx, (TickType_t)timeout) == pdFALSE) {
        err = I2_TIMEOUT;
      }
#else /* ENABLE_RTOS_AWARE_HAL */
      while (ctx->rx_status == I2_TRANSFER_WAIT);
#endif
  }

  /* RX ISR says the FIFO is at least half full or rx line is idle */
  if ( err != I2_TIMEOUT ) {
    i2_fifo_copy(&ctx->rx_fifo, rxbuf, &_size, false);
  } else {
    _size = 0;
  }

  /* Record correct number of received bytes */
  if ( num_bytes ) {
    *num_bytes = _size;
  }

  /* If we return without data, it's a timeout */
  if ( _size ) {
    err = I2_SUCCESS;
  } else {
    err = I2_TIMEOUT;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex_rx);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return err;
}

i2_error i2_uart_rx_polling(i2_uart_inst_t *inst, uint8_t *rxbuf, int32_t size,
                            int32_t *num_bytes, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  HAL_StatusTypeDef retval;
  i2_uart_ctx_t *ctx;
  UART_HandleTypeDef *huart;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->mutex_rx, (TickType_t)timeout) == pdFALSE) {
    if ( num_bytes ) {
      *num_bytes = 0;
    }
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* Get the pointer to the UART handle */
  huart = &(ctx->uart);

  retval = HAL_UART_Receive(huart, rxbuf, size, (TickType_t)timeout);

  err = i2_get_hal_error(retval);

  /* Record correct number of received bytes */
  if ( num_bytes ) {
    if ( (err == I2_SUCCESS) || (err == I2_TIMEOUT) ) {
      *num_bytes = (size - huart->RxXferCount);
    } else {
      *num_bytes = 0;
    }
  }

  if ( err != I2_SUCCESS ) {
    HAL_UART_AbortReceive(huart);
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex_rx);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return err;
}

#if defined ( I2_ENABLE_UART1_CONTEXT )
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(usart1);
}
#if ( UART1_DMA_ENABLE == I2_ENABLE )
void UART1_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart1->hdmarx);
}

void UART1_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart1->hdmatx);
}
#endif /* UART1_DMA_ENABLE */
#endif /* I2_ENABLE_UART1_CONTEXT */

#if defined ( I2_ENABLE_UART2_CONTEXT )
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(usart2);
}
#if ( UART2_DMA_ENABLE == I2_ENABLE )
void UART2_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart2->hdmarx);
}

void UART2_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart2->hdmatx);
}
#endif /* UART2_DMA_ENABLE */
#endif /* I2_ENABLE_UART2_CONTEXT */

#if defined ( I2_ENABLE_UART3_CONTEXT )
void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(usart3);
}
#if ( UART3_DMA_ENABLE == I2_ENABLE )
void UART3_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart3->hdmarx);
}

void UART3_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart3->hdmatx);
}
#endif /* UART3_DMA_ENABLE */
#endif /* I2_ENABLE_UART3_CONTEXT */

#if defined ( I2_ENABLE_UART4_CONTEXT )
void UART4_IRQHandler(void)
{
  HAL_UART_IRQHandler(uart4);
}
#if ( UART4_DMA_ENABLE == I2_ENABLE )
void UART4_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uart4->hdmarx);
}

void UART4_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uart4->hdmatx);
}
#endif /* UART4_DMA_ENABLE */
#endif /* I2_ENABLE_UART4_CONTEXT */

#if defined ( I2_ENABLE_UART5_CONTEXT )
void UART5_IRQHandler(void)
{
  HAL_UART_IRQHandler(uart5);
}
#if ( UART5_DMA_ENABLE == I2_ENABLE )
void UART5_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uart5->hdmarx);
}

void UART5_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uart5->hdmatx);
}
#endif /* UART5_DMA_ENABLE */
#endif /* I2_ENABLE_UART5_CONTEXT */

#if defined ( I2_ENABLE_UART6_CONTEXT )
void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(usart6);
}
#if ( UART6_DMA_ENABLE == I2_ENABLE )
void UART6_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart6->hdmarx);
}

void UART6_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(usart6->hdmatx);
}
#endif /* UART6_DMA_ENABLE */
#endif /* I2_ENABLE_UART6_CONTEXT */

#if 0
void UART_Receive_IT(UART_HandleTypeDef *huart)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  uint16_t uhdata;
  i2_uart_ctx_t *ctx = uart_get_ctx_from_handle(huart);
  int32_t count;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
    /* Reading RDR will clear the RXNE */
    uhdata = (uint16_t)READ_REG(huart->Instance->DR);

    if ( !ctx ) {
      return;
    }

    /* Read the RX byte */
    count = i2_fifo_write(&ctx->rx_fifo, (uint8_t) uhdata, true);

    /* Signal uart_rx that the FIFO is at the trigger level */
    if (count == ctx->rx_trigger_level) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
      xSemaphoreGiveFromISR(ctx->sem_rx, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
      ctx->rx_status = I2_TRANSFER_DONE;
    }

    /* Update the water mark */
    if (ctx->rx_water_mark < count) {
      ctx->rx_water_mark = count;
    }
  } else {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }

  return;
}

void UART_Idle_IT(UART_HandleTypeDef *huart)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
    static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
    struct i2_uart_ctx_t *ctx = uart_get_ctx_from_handle(huart);

    /* If RX line idle is detected, signal uart_rx */
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

    if (!ctx)
        return;

    if (ctx->rx_hal_mode == DMA_MODE) {
        ctx->rxfifo.wr_index = (I2_UART_FIFO_SIZE - ctx->hdma_rx.Instance->CNDTR);
    }

    if (fifo_count(&ctx->rxfifo, true)) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
        xSemaphoreGiveFromISR(ctx->sem_rx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
#endif /* ENABLE_RTOS_AWARE_HAL */
        ctx->rx_status = I2_TRANSFER_DONE;
    }

    return;
}
#endif /* 0 */

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  i2_uart_ctx_t *ctx = uart_get_ctx_from_handle(huart);

  if ( ctx ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    xSemaphoreGiveFromISR(ctx->sem_rx, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
    ctx->rx_status = I2_TRANSFER_DONE;
    ctx->rx_fifo.wr_index = (I2_UART_FIFO_SIZE - ctx->hdma_rx.Instance->NDTR);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  i2_uart_ctx_t *ctx = uart_get_ctx_from_handle(huart);

  if (ctx) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    xSemaphoreGiveFromISR(ctx->sem_rx, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
    ctx->rx_status = I2_TRANSFER_DONE;
    ctx->rx_fifo.wr_index = (I2_UART_FIFO_SIZE - ctx->hdma_rx.Instance->NDTR);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  i2_uart_ctx_t *ctx = uart_get_ctx_from_handle(huart);

  if (ctx) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    xSemaphoreGiveFromISR(ctx->sem_tx, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
    ctx->tx_status = I2_TRANSFER_DONE;
  }
}

i2_error i2_uart_rx_trigger_level_set(i2_uart_inst_t *inst, int32_t rx_trigger_level)
{
  i2_error err = I2_SUCCESS;
  i2_uart_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  if ( !ctx->rx_buffering_on ) {
    ctx->rx_trigger_level = rx_trigger_level;
  } else {
    err = I2_BUSY;
  }

  return err;
}

i2_error i2_uart_rx_trigger_level_get(i2_uart_inst_t *inst,
                                      int32_t *rx_trigger_level)
{
  i2_uart_ctx_t *ctx;

  if ( !inst || !rx_trigger_level ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  *rx_trigger_level = ctx->rx_trigger_level;

  return I2_SUCCESS;
}

i2_error i2_uart_rx_water_mark_get(i2_uart_inst_t *inst, int32_t *rx_water_mark)
{
  i2_uart_ctx_t *ctx;

  if ( !inst || !rx_water_mark ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  *rx_water_mark = ctx->rx_water_mark;

  return I2_SUCCESS;
}

i2_error i2_uart_rx_byte_count_get(i2_uart_inst_t *inst, int32_t *count)
{
  i2_uart_ctx_t *ctx;

  if ( !inst || !count ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  *count = i2_fifo_count(&ctx->rx_fifo, false);

  return I2_SUCCESS;
}

i2_error i2_uart_rx_fifo_size_get(i2_uart_inst_t *inst, int32_t *size)
{
  i2_uart_ctx_t *ctx;

  if ( !inst || !size ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  *size = i2_fifo_size(&ctx->rx_fifo);

  return I2_SUCCESS;
}

i2_error i2_uart_baud_rate_set(i2_uart_inst_t *inst, int32_t baud_rate)
{
  i2_uart_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = uart_get_ctx_from_name(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  ctx->baud_rate = baud_rate;

  return I2_SUCCESS;
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
