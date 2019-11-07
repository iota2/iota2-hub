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
 * @file        i2_stm32f4xx_hal_spi.c
 * @brief       SPI set-up and control.
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
#include "i2_stm32f4xx_hal_spi.h"

#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_gpio_ex.h"

/* Private defines -----------------------------------------------------------*/
/* Definition for SPI Priority */
#define SPI_PREEMPTION_PRIORITY       ( 5 ) /**< SPI preemption priority      */
#define SPI_SUB_PRIORITY              ( 1 ) /**< SPI sub priority             */
#define SPI_DMA_PREEMPTION_PRIORITY   ( 5 ) /**< SPI DMA preemption priority  */
#define SPI_DMA_SUB_PRIORITY          ( 1 ) /**< SPI DMA sub priority         */

/**
 * @defgroup I2_SPI_CONFIG SPI configurations.
 * Defines available configurations for SPI peripheral  including GPIO and DMA.
 *
 * @{
 *
 *  | SPI     | PIN    | GPIO    | AF  | MODE  | DMA   | CHANNEL   | STREAM    |
 *  |:-------:|:------:|:-------:|:---:|:-----:|:-----:|:---------:|:---------:|
 *  | SPI1    | NSS    | PA4     | AF5 |       |       |           |           |
 *  | ^       | ^      | PA15    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | SCLK   | PA5     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB3     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MISO   | PA6     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB4     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MOSI   | PA7     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB5     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       |        |         | ^   | RX    | DMA2  | CHANNEL3  | STREAM0   |
 *  | ^       | ^      | ^       | ^   | ^     | ^     | ^         | STREAM2   |
 *  | ^       |        |         | ^   | TX    | ^     | ^         | STREAM3   |
 *  | ^       | ^      | ^       | ^   | ^     | ^     | ^         | STREAM4   |
 *  | SPI2    | NSS    | PB9     | AF5 |       |       |           |           |
 *  | ^       | ^      | PB12    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PI0     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | SCLK   | PB10    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB13    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PI1     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MISO   | PC2     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB14    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PI2     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MOSI   | PC3     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB15    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PI3     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       |        |         | ^   | RX    | DMA1  | CHANNEL0  | STREAM3   |
 *  | ^       |        |         | ^   | TX    | ^     | ^         | STREAM4   |
 *  | SPI3    | NSS    | PA4     | AF5 |       |       |           |           |
 *  | ^       | ^      | PA15    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | SCLK   | PC10    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB3     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MISO   | PC11    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB4     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | MOSI   | PC12    | ^   | ^     | ^     | ^         | ^         |
 *  | ^       | ^      | PB5     | ^   | ^     | ^     | ^         | ^         |
 *  | ^       |        |         | ^   | RX    | DMA1  | CHANNEL0  | STREAM0   |
 *  | ^       | ^      | ^       | ^   | ^     | ^     | ^         | STREAM2   |
 *  | ^       |        |         | ^   | TX    | ^     | ^         | STREAM5   |
 *  | ^       | ^      | ^       | ^   | ^     | ^     | ^         | STREAM7   |
 */
/** @} */ /* I2_SPI_CONFIG */

/**
 * @defgroup I2_ENABLE_SPI_CONTEXT SPI peripherals enabling.
 * Enables any number of SPI context among all available SPI peripherals.
 *
 * @{
 */
#define I2_MAX_NUM_SPI_CONTEXT    ( 3 ) /**< Count of SPI context to enable   */
#define I2_ENABLE_SPI1_CONTEXT          /**< Define to enable SPI1 context    */
#define I2_ENABLE_SPI2_CONTEXT          /**< Define to enable SPI2 context    */
#define I2_ENABLE_SPI3_CONTEXT          /**< Define to enable SPI3 context    */
/** @} */ /* I2_ENABLE_SPI_CONTEXT */

/* SPI1 CONFIGURATION SELECTION ***********************************************/
#if defined ( I2_ENABLE_SPI1_CONTEXT )
/**
 * @defgroup I2_SPI1_CONFIG SPI1 configurations.
 * Configuration settings for SPI1.
 *
 * @{
 */
/**
 * @defgroup I2_SPI1_GPIO SPI1 pin configurations.
 * Definition for SPI1 Pins, to be defined as < PORT, PIN >.
 *
 * @{
 */
#define SPI1_SCLK_GPIO_CONFIG   GPIOB, GPIO_PIN_3   /**< SPI1 SCKL pin config */
#define SPI1_MISO_GPIO_CONFIG   GPIOB, GPIO_PIN_4   /**< SPI1 MISO pin config */
#define SPI1_MOSI_GPIO_CONFIG   GPIOB, GPIO_PIN_5   /**< SPI1 MOSI pin config */
/** @} */ /* I2_SPI1_GPIO */
/**
 * @defgroup I2_SPI1_DMA SPI1 DMA configurations.
 * Definition for SPI1 DMA, to be defined as < CHANNEL, STREAM >.
 *
 * @{
 */
#define SPI1_RX_DMA_CONFIG      DMA_CHANNEL_3, DMA2_Stream2   /**< SPI1 RxDMA */
#define SPI1_TX_DMA_CONFIG      DMA_CHANNEL_3, DMA2_Stream3   /**< SPI1 TxDMA */
/** @} */ /* I2_SPI1_DMA */
/**
 * @defgroup I2_SPI1_DMA_IRQn SPI1 interrupt configurations.
 * Definition for SPI1 DMA interrupt handlers.
 *
 * @{
 */
#define SPI1_DMA_RX_IRQn        DMA2_Stream2_IRQn       /**< SPI1 DMA RX IRQn */
#define SPI1_DMA_TX_IRQn        DMA2_Stream3_IRQn       /**< SPI1 DMA TX IRQn */
#define SPI1_DMA_RX_IRQHandler  DMA2_Stream2_IRQHandler /**< SPI1 RX Handler  */
#define SPI1_DMA_TX_IRQHandler  DMA2_Stream3_IRQHandler /**< SPI1 TX Handler  */
/** @} */ /* I2_SPI1_DMA_IRQn */
/** @} */ /* I2_SPI1_CONFIG */
#endif    /* I2_ENABLE_SPI1_CONTEXT */
/******************************************************************************/

/* SPI2 CONFIGURATION SELECTION ***********************************************/
#if defined ( I2_ENABLE_SPI2_CONTEXT )
/**
 * @defgroup I2_SPI2_CONFIG SPI2 configurations.
 * Configuration settings for SPI2.
 *
 * @{
 */
/**
 * @defgroup I2_SPI2_GPIO SPI2 pin configurations.
 * Definition for SPI2 Pins, to be defined as < PORT, PIN >.
 *
 * @{
 */
#define SPI2_SCLK_GPIO_CONFIG   GPIOI, GPIO_PIN_1   /**< SPI2 SCKL pin config */
#define SPI2_MISO_GPIO_CONFIG   GPIOI, GPIO_PIN_2   /**< SPI2 MISO pin config */
#define SPI2_MOSI_GPIO_CONFIG   GPIOI, GPIO_PIN_3   /**< SPI2 MOSI pin config */
/** @} */ /* I2_SPI2_GPIO */
/**
 * @defgroup I2_SPI2_DMA SPI2 DMA configurations.
 * Definition for SPI2 DMA, to be defined as < CHANNEL, STREAM >.
 *
 * @{
 */
#define SPI2_RX_DMA_CONFIG      DMA_CHANNEL_0, DMA1_Stream3 /**< SPI2 RX DMA  */
#define SPI2_TX_DMA_CONFIG      DMA_CHANNEL_0, DMA1_Stream4 /**< SPI2 TX DMA  */
/** @} */ /* I2_SPI2_DMA */
/**
 * @defgroup I2_SPI2_DMA_IRQn SPI2 interrupt configurations.
 * Definition for SPI2 DMA interrupt handlers.
 *
 * @{
 */
#define SPI2_DMA_RX_IRQn        DMA1_Stream3_IRQn       /**< SPI2 DMA RX IRQn */
#define SPI2_DMA_TX_IRQn        DMA1_Stream4_IRQn       /**< SPI2 DMA TX IRQn */
#define SPI2_DMA_RX_IRQHandler  DMA1_Stream3_IRQHandler /**< SPI2 RX Handler  */
#define SPI2_DMA_TX_IRQHandler  DMA1_Stream4_IRQHandler /**< SPI2 TX Handler  */
/** @} */ /* I2_SPI2_DMA_IRQn */
/** @} */ /* I2_SPI2_CONFIG */
#endif    /* I2_ENABLE_SPI2_CONTEXT */
/******************************************************************************/

/* SPI3 CONFIGURATION SELECTION ***********************************************/
#if defined ( I2_ENABLE_SPI3_CONTEXT )
/**
 * @defgroup I2_SPI3_CONFIG SPI3 configurations.
 * Configuration settings for SPI3.
 *
 * @{
 */
/**
 * @defgroup I2_SPI3_GPIO SPI3 pin configurations.
 * Definition for SPI3 Pins, to be defined as < PORT, PIN >.
 *
 * @{
 */
#define SPI3_SCLK_GPIO_CONFIG   GPIOC, GPIO_PIN_10  /**< SPI3 SCKL pin config */
#define SPI3_MISO_GPIO_CONFIG   GPIOC, GPIO_PIN_11  /**< SPI3 MISO pin config */
#define SPI3_MOSI_GPIO_CONFIG   GPIOC, GPIO_PIN_12  /**< SPI3 MOSI pin config */
/** @} */ /* I2_SPI3_GPIO */
/**
 * @defgroup I2_SPI3_DMA SPI3 DMA configurations.
 * Definition for SPI3 DMA, to be defined as < CHANNEL, STREAM >.
 *
 * @{
 */
#define SPI3_RX_DMA_CONFIG      DMA_CHANNEL_0, DMA1_Stream2 /**< SPI3 RX DMA  */
#define SPI3_TX_DMA_CONFIG      DMA_CHANNEL_0, DMA1_Stream7 /**< SPI3 TX DMA  */
/** @} */ /* I2_SPI3_DMA */
/**
 * @defgroup I2_SPI3_DMA_IRQn SPI3 interrupt configurations.
 * Definition for SPI3 DMA interrupt handlers.
 *
 * @{
 */
#define SPI3_DMA_RX_IRQn        DMA1_Stream2_IRQn       /**< SPI3 DMA RX IRQn */
#define SPI3_DMA_TX_IRQn        DMA1_Stream7_IRQn       /**< SPI3 DMA TX IRQn */
#define SPI3_DMA_RX_IRQHandler  DMA1_Stream2_IRQHandler /**< SPI3 RX Handler  */
#define SPI3_DMA_TX_IRQHandler  DMA1_Stream7_IRQHandler /**< SPI3 TX Handler  */
/** @} */ /* I2_SPI3_DMA_IRQn */
/** @} */ /* I2_SPI3_CONFIG */
#endif    /* I2_ENABLE_SPI3_CONTEXT */
/******************************************************************************/

/**
 * @defgroup i2_spi_ctx_t SPI peripheral context.
 * This defines the attributes of SPI hardware controller. Note that
 * a SPI hardware controller can be shared by many SPI device instances
 * with different chip select GPIO. The details of the controller attributes
 * are statically defined in the spi.c. We do not have any need to dynamically
 * configure them yet nor do we want to do that as they might be shared.
 *
 * @{
 */
/** @brief SPI context */
typedef struct {
  const char*           ctx_name;         /**< SPI context name               */
  i2_gpio_inst_t        SCLK;             /**< GPIO pin for SPI SCLK          */
  i2_gpio_inst_t        MISO;             /**< GPIO pin for SPI MISO          */
  i2_gpio_inst_t        MOSI;             /**< GPIO pin for SPI MOSI          */
  int32_t               hal_mode;         /**< SPI HAL operational mode       */
  bool                  initialized;      /**< SPI initialization flag        */
  SPI_TypeDef*          baseaddr;         /**< SPI peripheral base address    */
  uint32_t              dma_rx_channel;   /**< SPI DMA RX channel             */
  DMA_Stream_TypeDef*   dma_rx_stream;    /**< SPI DMA RX stream              */
  uint32_t              dma_rx_priority;  /**< SPI DMA RX channel priority    */
  IRQn_Type             dma_rx_irqn;      /**< SPI DMA RX interrupt request   */
  uint32_t              dma_tx_channel;   /**< SPI DMS TX channel             */
  DMA_Stream_TypeDef*   dma_tx_stream;    /**< SPI DMA TX stream              */
  uint32_t              dma_tx_priority;  /**< SPI DMA TX channel priority    */
  IRQn_Type             dma_tx_irqn;      /**< SPI DMA RX interrupt request   */
  SPI_HandleTypeDef     spi;              /**< SPI peripheral handler         */
  DMA_HandleTypeDef     hdma_rx;          /**< SPI DMA handler for RX channel */
  DMA_HandleTypeDef     hdma_tx;          /**< SPI DMA handler for TX channel */
 #if defined ( ENABLE_RTOS_AWARE_HAL)
  SemaphoreHandle_t     mutex;            /**< SPI context mutex              */
  SemaphoreHandle_t     sem;              /**< SPI context semaphore          */
#endif /* ENABLE_RTOS_AWARE_HAL */
  __IO ITStatus         status;           /**< SPI interrupt status           */
} i2_spi_ctx_t;
/** @} */ /* i2_spi_ctx_t */

/** @brief SPI Context used in application */
static i2_spi_ctx_t i2_spi_ctx_table[I2_MAX_NUM_SPI_CONTEXT] = {
#if defined ( I2_ENABLE_SPI1_CONTEXT )
  {
    "SPI1",
    { "spi1_sclk", SPI1_SCLK_GPIO_CONFIG },
    { "spi1_miso", SPI1_MISO_GPIO_CONFIG },
    { "spi1_mosi", SPI1_MOSI_GPIO_CONFIG },
    DMA_MODE, false, SPI1,
    SPI1_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     SPI1_DMA_RX_IRQn,
    SPI1_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   SPI1_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_SPI1_CONTEXT */
#if defined ( I2_ENABLE_SPI2_CONTEXT )
  {
    "SPI2",
    { "spi2_sclk", SPI2_SCLK_GPIO_CONFIG },
    { "spi2_miso", SPI2_MISO_GPIO_CONFIG },
    { "spi2_mosi", SPI2_MOSI_GPIO_CONFIG },
    DMA_MODE, false, SPI2,
    SPI2_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     SPI2_DMA_RX_IRQn,
    SPI2_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   SPI2_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_SPI2_CONTEXT */
#if defined ( I2_ENABLE_SPI3_CONTEXT )
  {
    "SPI3",
    { "spi3_sclk", SPI3_SCLK_GPIO_CONFIG },
    { "spi3_miso", SPI3_MISO_GPIO_CONFIG },
    { "spi3_mosi", SPI3_MOSI_GPIO_CONFIG },
    DMA_MODE, false, SPI3,
    SPI3_RX_DMA_CONFIG, DMA_PRIORITY_HIGH,     SPI3_DMA_RX_IRQn,
    SPI3_TX_DMA_CONFIG, DMA_PRIORITY_MEDIUM,   SPI3_DMA_TX_IRQn,
  },
#endif /* I2_ENABLE_SPI3_CONTEXT */
};

/**
 * @brief Number of SPI instances to enable.
 * @details We can have more number of SPI device instances
 *          than the number of SPI controllers.
 */
#define I2_MAX_NUM_SPI_INSTANCE    (3)
/** @brief Table to maintain all enabled SPI instances */
static i2_spi_inst_t *i2_spi_inst_table[I2_MAX_NUM_SPI_INSTANCE];

#if defined ( I2_ENABLE_SPI1_CONTEXT )
static SPI_HandleTypeDef *spi1 = NULL;              /**< SPI1 control handler */
#endif /* I2_ENABLE_SPI1_CONTEXT */
#if defined ( I2_ENABLE_SPI2_CONTEXT )
static SPI_HandleTypeDef *spi2 = NULL;              /**< SPI2 control handler */
#endif /* I2_ENABLE_SPI2_CONTEXT */
#if defined ( I2_ENABLE_SPI3_CONTEXT )
static SPI_HandleTypeDef *spi3 = NULL;              /**< SPI3 control handler */
#endif /* I2_ENABLE_SPI3_CONTEXT */

/* Private functions ---------------------------------------------------------*/
/**
 * @brief   SPI instance to context converter.
 * @details Get SPI context from corresponding instance.
 *
 * @param[in] *name       SPI instance name.
 * @return  SPI context object.
 */
static i2_spi_ctx_t* spi_inst_to_ctx(const char *name)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_SPI_CONTEXT; i++) {
    if (!strcmp(i2_spi_ctx_table[i].ctx_name, name)) {
      return &i2_spi_ctx_table[i];
    }
  }
  return NULL;
}

/**
 * @brief   SPI handle to context converter.
 * @details Get SPI context from corresponding HAL handle.
 *
 * @param[in] *hspi       SPI HAL handle.
 * @return  SPI context object.
 */
static i2_spi_ctx_t* spi_handle_to_ctx(SPI_HandleTypeDef *hspi)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_SPI_CONTEXT; i++) {
    if (&i2_spi_ctx_table[i].spi == hspi) {
      return &i2_spi_ctx_table[i];
    }
  }
  return NULL;
}

/* Public functions ----------------------------------------------------------*/
/**
 * @brief   SPI initialization.
 * @details Initializes a SPI instance along with GPIO, DMA and interrupts.
 *
 * @param[in] *inst       SPI instance to initialize.
 * @return  Initialization error code @ref I2_ERROR.
 */
i2_error i2_spi_init(i2_spi_inst_t *inst)
{
  i2_error retval = I2_SUCCESS;
  i2_spi_ctx_t *ctx;
  SPI_HandleTypeDef *hspi;
  SPI_TypeDef *base;
  IRQn_Type irqn;
  int32_t i;
  uint32_t alt;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  /* get SPI context from name */
  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Create mutexes. */
  ctx->mutex = xSemaphoreCreateMutex();

  /* Create semaphores */
  ctx->sem = xSemaphoreCreateBinary();
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* SPI driver needs GPIO */
  i2_gpio_init();

  /* Configure CS */
  if (i2_gpio_is_valid(&inst->CS)) {
    i2_gpio_config_out(&inst->CS, I2_HIGH);
  }

  /*
   * If context has been already initialized,
   * only init SPI device instance specific portions.
   */
  if ( ctx->initialized ) {
    for (i = 0; i < I2_MAX_NUM_SPI_INSTANCE; i++) {
      if (!i2_spi_inst_table[i]) {
        i2_spi_inst_table[i] = inst;
        break;
      }
    }
    return I2_SUCCESS;
  }

  /* Get the pointer to the SPI handle */
  hspi = &(ctx->spi);

  /* Get the SPI controller base address */
  base = ctx->baseaddr;

  ctx->status = I2_TRANSFER_DONE;

  if ( base == SPI1 ) {
    spi1 = hspi;
    irqn = SPI1_IRQn;
    alt = GPIO_AF5_SPI1;
    __HAL_RCC_SPI1_CLK_ENABLE();
    if ( ctx->hal_mode == DMA_MODE ) {
      __HAL_RCC_DMA2_CLK_ENABLE();
    }
  } else if ( base == SPI2 ) {
    spi2 = hspi;
    irqn = SPI2_IRQn;
    alt = GPIO_AF5_SPI2;
    __HAL_RCC_SPI2_CLK_ENABLE();
    if ( ctx->hal_mode == DMA_MODE ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
  } else if ( base == SPI3 ) {
    spi3 = hspi;
    irqn = SPI3_IRQn;
    alt = GPIO_AF6_SPI3;
    __HAL_RCC_SPI3_CLK_ENABLE();
    if ( ctx->hal_mode == DMA_MODE ) {
      __HAL_RCC_DMA1_CLK_ENABLE();
    }
  } else {
    return I2_INVALID_PARAM;
  }

  /* Configure SCLK */
  if ( i2_gpio_is_valid(&ctx->SCLK) ) {
    i2_gpio_config_alt(&ctx->SCLK, GPIO_MODE_AF_PP, GPIO_PULLDOWN, alt);
  }

  /* Configure MISO */
  if ( i2_gpio_is_valid(&ctx->MISO) ) {
    i2_gpio_config_alt(&ctx->MISO, GPIO_MODE_AF_PP, GPIO_PULLDOWN, alt);
  }

  /* Configure MOSI */
  if ( i2_gpio_is_valid(&ctx->MOSI) ) {
    i2_gpio_config_alt(&ctx->MOSI, GPIO_MODE_AF_PP, GPIO_PULLDOWN, alt);
  }

  /* DMA RX Config */
  if ( ctx->hal_mode == DMA_MODE) {
    /*##### Configure the DMA streams ########################################*/
    /* Configure the DMA handler for Transmission process */
    ctx->hdma_rx.Instance                 = ctx->dma_rx_stream;
    ctx->hdma_rx.Init.Channel             = ctx->dma_rx_channel;
    ctx->hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    ctx->hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    ctx->hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    ctx->hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ctx->hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    ctx->hdma_rx.Init.Mode                = DMA_NORMAL;
    ctx->hdma_rx.Init.Priority            = ctx->dma_rx_priority;
    ctx->hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    ctx->hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    ctx->hdma_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
    ctx->hdma_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

    HAL_DMA_Init(&ctx->hdma_rx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, ctx->hdma_rx);

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
    ctx->hdma_tx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

    HAL_DMA_Init(&ctx->hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, ctx->hdma_tx);

    /*##### Configure the NVIC for DMA #######################################*/
    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority( ctx->dma_rx_irqn, SPI_DMA_PREEMPTION_PRIORITY,
                          SPI_DMA_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ctx->dma_rx_irqn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority( ctx->dma_tx_irqn,
                          SPI_DMA_PREEMPTION_PRIORITY, SPI_DMA_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(ctx->dma_tx_irqn);
  }

  HAL_NVIC_SetPriority(irqn, SPI_PREEMPTION_PRIORITY, SPI_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(irqn);

  /*##### Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  hspi->Instance                  = base;
  /* Clock speed defaults to 20MHZ (80Mhz / 4) */
  hspi->Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_4;
  hspi->Init.Direction            = SPI_DIRECTION_2LINES;
  /* Default to SPI_MODE_0 */
  hspi->Init.CLKPhase             = SPI_PHASE_1EDGE;
  hspi->Init.CLKPolarity          = SPI_POLARITY_LOW;
  /* Data width defaults to 8bit */
  hspi->Init.DataSize             = SPI_DATASIZE_8BIT;
  /* Bit shift starts with the highest bit of the data unit by default */
  hspi->Init.FirstBit             = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode               = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial        = 7;
  hspi->Init.NSS                  = SPI_NSS_SOFT;
  hspi->Init.Mode                 = SPI_MODE_MASTER;

  if ( HAL_SPI_Init(hspi) != HAL_OK ) {
    retval = I2_FAILURE;
  } else {
    for (i = 0; i < I2_MAX_NUM_SPI_INSTANCE; i++) {
      if ( !i2_spi_inst_table[i] ) {
        i2_spi_inst_table[i] = inst;
        break;
      }
    }
  }

  ctx->initialized = true;

  return retval;
}

/**
 * @brief   SPI deinitialization.
 * @details Deinitializes and SPI instance.
 *
 * @param[in] *inst         SPI instance to deinitialize.
 * @return  Deinitialization error code @ref I2_ERROR.
 */
i2_error i2_spi_deinit(i2_spi_inst_t *inst)
{
  i2_spi_ctx_t *ctx;
  SPI_TypeDef *base;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  /* get SPI context from name */
  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Delete mutexes. */
  vSemaphoreDelete( ctx->mutex );

  /* Delete semaphores */
  vSemaphoreDelete(ctx->sem);
#endif /* ENABLE_RTOS_AWARE_HAL */

  base = ctx->baseaddr;
  /*##### Reset peripherals and Disable NVIC #################################*/
  if ( base == SPI1 ) {
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();
    HAL_NVIC_DisableIRQ(SPI1_DMA_RX_IRQn);
    HAL_NVIC_DisableIRQ(SPI1_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  } else if ( base == SPI2 ) {
    __HAL_RCC_SPI2_FORCE_RESET();
    __HAL_RCC_SPI2_RELEASE_RESET();
    HAL_NVIC_DisableIRQ(SPI2_DMA_RX_IRQn);
    HAL_NVIC_DisableIRQ(SPI2_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(SPI2_IRQn);
  } else if ( base == SPI3 ) {
    __HAL_RCC_SPI3_FORCE_RESET();
    __HAL_RCC_SPI3_RELEASE_RESET();
    HAL_NVIC_DisableIRQ(SPI3_DMA_RX_IRQn);
    HAL_NVIC_DisableIRQ(SPI3_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(SPI3_IRQn);
  } else {
    return I2_NOT_AVAILABLE;
  }

  /*##### Disable peripherals and GPIO Clocks ################################*/
  i2_gpio_release(&ctx->SCLK);
  i2_gpio_release(&ctx->MISO);
  i2_gpio_release(&ctx->MOSI);

  /*##### Disable the DMA Streams ############################################*/
  HAL_DMA_DeInit(&ctx->hdma_tx);
  HAL_DMA_DeInit(&ctx->hdma_rx);

  return I2_SUCCESS;
}

/**
 * @brief   SPI instance get.
 * @details Returns SPI instance object from name.
 *
 * @param[in] *inst_name       Name of instance to get.
 * @return  SPI instance object corresponding to passed name.
 */
i2_spi_inst_t* i2_spi_inst_get(char *inst_name)
{
  int32_t i;

  for (i = 0; i < I2_MAX_NUM_SPI_INSTANCE; i++) {
    if ( !strcmp(i2_spi_inst_table[i]->inst_name, inst_name) ) {
      return i2_spi_inst_table[i];
    }
  }

  return NULL;
}

/**
 * @brief   SPI setting configuration.
 * @details Configures various settings for SPI peripheral.
 *
 * @param[in] *ctx        SPI context.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @return  Execution error code @ref I2_ERROR.
 */
static i2_error spi_config(i2_spi_ctx_t *ctx, i2_spi_data_width_t data_width,
                           i2_spi_clock_speed_t clk_speed,
                           i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit)
{
  i2_error retval = I2_SUCCESS;
  SPI_HandleTypeDef *hspi;
  uint32_t prescaler;

  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* SPI controller needs to be initialized first */
  if ( !ctx->initialized ) {
    return I2_FAILURE;
  }

  /* Get the pointer to the SPI hspi */
  hspi = &(ctx->spi);

  /* Set the SPI mode */
  switch ( spi_mode ) {
  case I2_SPI_MODE_0 :
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    break;
  case I2_SPI_MODE_1 :
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    break;
  case I2_SPI_MODE_2 :
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    break;
  case I2_SPI_MODE_3 :
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    break;
  default:
    return I2_INVALID_PARAM;
  }

  /* Set the SPI clock */
  switch ( clk_speed ) {
  case I2_SPI_CLK_40_MHZ:
    prescaler = SPI_BAUDRATEPRESCALER_2;
    break;
  case I2_SPI_CLK_20_MHZ:
    prescaler = SPI_BAUDRATEPRESCALER_4;
    break;
  case I2_SPI_CLK_10_MHZ:
    prescaler = SPI_BAUDRATEPRESCALER_8;
    break;
  case I2_SPI_CLK_5_MHZ:
    prescaler = SPI_BAUDRATEPRESCALER_16;
    break;
  case I2_SPI_CLK_2500_KHZ:
    prescaler = SPI_BAUDRATEPRESCALER_32;
    break;
  case I2_SPI_CLK_1250_KHZ:
    prescaler = SPI_BAUDRATEPRESCALER_64;
    break;
  case I2_SPI_CLK_625_KHZ:
    prescaler = SPI_BAUDRATEPRESCALER_128;
    break;
  case I2_SPI_CLK_313_KHZ:
    prescaler = SPI_BAUDRATEPRESCALER_256;
    break;
  default:
    return I2_INVALID_PARAM;
  }
  hspi->Init.BaudRatePrescaler = prescaler;

  /* Set the SPI data width */
  switch ( data_width ) {
  case I2_SPI_DATA_WIDTH_8BIT:
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    break;
  case I2_SPI_DATA_WIDTH_16BIT:
    hspi->Init.DataSize = SPI_DATASIZE_16BIT;
    break;
  default:
    return I2_INVALID_PARAM;
  }

  /* Set first bit shift order */
  switch ( first_bit ) {
  case I2_SPI_MSBIT_FIRST:
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    break;
  case I2_SPI_LSBIT_FIRST:
    hspi->Init.FirstBit = SPI_FIRSTBIT_LSB;
    break;
  default:
    return I2_INVALID_PARAM;
  }

  /* This will re-init the SPI controller */
  if ( HAL_SPI_Init(hspi) != HAL_OK ) {
    retval = I2_FAILURE;
  }

  return retval;
}

/**
 * @brief   SPI send / receive.
 * @details Transmits and receives data on SPI bus, with CS already asserted.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[in] *txbuf      If null, then this function will only receive data on
 *                        SPI bus, and put it in rxbuf, else this buffer will be
 *                        transmitted on SPI bus.
 * @param[out] *rxbuf     If null, then this function will only transmit data
 *                        form txbuf on SPI bus, else this buffer will be used
 *                        to receive data on SPI bus.
 * @param[in] size        Amount of data to be transmitted / received.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 *
 * @note    If both txbuf and rxbuf are non null, then txbuf will be transmitted
 *          TX pin and on same clock data will be received on RX pin and will be
 *          put in rxbuf.
 */
i2_error i2_spi_txrx_raw( i2_spi_inst_t *inst, i2_spi_data_width_t data_width,
                          i2_spi_clock_speed_t clk_speed,
                          i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                          uint8_t *txbuf, uint8_t *rxbuf,
                          int32_t size, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  HAL_StatusTypeDef retval;
  i2_spi_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  err = spi_config(ctx, data_width, clk_speed, spi_mode, first_bit);
  if ( err != I2_SUCCESS ) {
    return err;
  }

  if ( ctx->hal_mode != POLLING_MODE ) {
    ctx->status = I2_TRANSFER_WAIT;
  }

  if (ctx->hal_mode == INTERRUPT_MODE) {
    if ( !rxbuf ) {
      retval = HAL_SPI_Transmit_IT(&ctx->spi, txbuf, size);
    } else if ( !txbuf ) {
      retval = HAL_SPI_Receive_IT(&ctx->spi, rxbuf, size);
    } else {
      retval = HAL_SPI_TransmitReceive_IT(&ctx->spi, txbuf, rxbuf, size);
    }
  } else if (ctx->hal_mode == DMA_MODE) {
    if ( !rxbuf ) {
      retval = HAL_SPI_Transmit_DMA(&ctx->spi, txbuf, size);
    } else if ( !txbuf ) {
      retval = HAL_SPI_Receive_DMA(&ctx->spi, rxbuf, size);
    } else {
      retval = HAL_SPI_TransmitReceive_DMA(&ctx->spi, txbuf, rxbuf, size);
    }
    return I2_NOT_IMPLEMENTED;
  } else {
    retval = HAL_SPI_TransmitReceive(&ctx->spi, txbuf, rxbuf, size,
                                      (TickType_t) timeout);
  }

  if ( ctx->hal_mode != POLLING_MODE ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    if (xSemaphoreTake(ctx->sem, (TickType_t)timeout) == pdFALSE) {
      retval = HAL_TIMEOUT;
    }
#else
    while (ctx->status == I2_TRANSFER_WAIT);
#endif /* ENABLE_RTOS_AWARE_HAL */
  }

  err = i2_get_hal_error(retval);

  /* Check for error interrupt */
  if ( ctx->status == I2_TRANSFER_ERROR ) {
    err = I2_FAILURE;
  } else if ( ctx->status == I2_TRANSFER_WAIT ) {
    /* Complete interrupt was not received */
    err = I2_TIMEOUT;
  }  else if ( txbuf && ctx->spi.TxXferCount ) {
    /* Check if the tx xfer is done */
    err = I2_FAILURE;
  } else if ( rxbuf && ctx->spi.RxXferCount ) {
    /* Check if the rx xfer is done */
    err = I2_FAILURE;
  }

  return err;
}

/**
 * @brief   SPI send / receive.
 * @details Transmits and receives data on SPI bus, This function will asserts
 *          and deasserts the CS pin.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[in] *txbuf      If null, then this function will only receive data on
 *                        SPI bus, and put it in rxbuf, else this buffer will be
 *                        transmitted on SPI bus.
 * @param[out] *rxbuf     If null, then this function will only transmit data
 *                        form txbuf on SPI bus, else this buffer will be used
 *                        to receive data on SPI bus.
 * @param[in] size        Amount of data to be transmitted / received.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 *
 * @note    If both txbuf and rxbuf are non null, then txbuf will be transmitted
 *          TX pin and on same clock data will be received on RX pin and will be
 *          put in rxbuf.
 */
i2_error i2_spi_txrx( i2_spi_inst_t *inst, i2_spi_data_width_t data_width,
                      i2_spi_clock_speed_t clk_speed,
                      i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                      uint8_t *txbuf, uint8_t *rxbuf,
                      int32_t size, uint32_t timeout)
{
  i2_error err = I2_FAILURE;
  i2_spi_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->mutex, (TickType_t)timeout) == pdFALSE) {
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL*/

  /* assert CS */
  if ( i2_gpio_is_valid(&inst->CS) ) {
    i2_gpio_set(&inst->CS, I2_LOW);
  }

  err = i2_spi_txrx_raw(inst, data_width, clk_speed, spi_mode,
                        first_bit, txbuf, rxbuf, size, timeout);

  /* deassert CS */
  i2_gpio_set(&inst->CS, I2_HIGH);

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return err;
}

/**
 * @brief   SPI send.
 * @details Transmits data on SPI bus, This function will asserts
 *          and deasserts the CS pin.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[in] *txbuf      This buffer will be transmitted on SPI bus.
 * @param[in] size        Amount of data to be transmitted.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_tx( i2_spi_inst_t *inst,  i2_spi_data_width_t data_width,
                    i2_spi_clock_speed_t clk_speed,
                    i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                    uint8_t *txbuf, int32_t size, uint32_t timeout)
{
  return i2_spi_txrx( inst, data_width, clk_speed, spi_mode,
                      first_bit, txbuf, NULL, size, timeout);
}

/**
 * @brief   SPI send.
 * @details Transmits data on SPI bus, This function will not asserts
 *          and deasserts the CS pin.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[in] *txbuf      This buffer will be transmitted on SPI bus.
 * @param[in] size        Amount of data to be transmitted.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_tx_raw( i2_spi_inst_t *inst,  i2_spi_data_width_t data_width,
                        i2_spi_clock_speed_t clk_speed,
                        i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                        uint8_t *txbuf, int32_t size, uint32_t timeout)
{
  return i2_spi_txrx_raw( inst, data_width, clk_speed, spi_mode,
                          first_bit, txbuf, NULL, size, timeout);
}

/**
 * @brief   SPI receive.
 * @details Receives data on SPI bus, This function will asserts
 *          and deasserts the CS pin.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[out] *rxbuf     This buffer will be used to receive data on SPI bus.
 * @param[in] size        Amount of data to be received.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_rx( i2_spi_inst_t *inst,  i2_spi_data_width_t data_width,
                    i2_spi_clock_speed_t clk_speed,
                    i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                    uint8_t *rxbuf, int32_t size, uint32_t timeout)
{
  return i2_spi_txrx( inst, data_width, clk_speed, spi_mode,
                      first_bit, NULL, rxbuf, size, timeout);
}

/**
 * @brief   SPI receive.
 * @details Receives data on SPI bus, This function will not asserts
 *          and deasserts the CS pin.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] data_width  Size of data bits for SPI @ref i2_spi_data_width_t.
 * @param[in] clk_speed   SPI clock speed @ref i2_spi_clock_speed_t.
 * @param[in] spi_mode    SPI communication mode @ref i2_spi_mode_t.
 * @param[in] first_bit   Communication start bit @ref i2_spi_first_bit_t.
 * @param[out] *rxbuf     This buffer will be used to receive data on SPI bus.
 * @param[in] size        Amount of data to be received.
 * @param[in] timeout     SPI bus HAL timeout.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_rx_raw( i2_spi_inst_t *inst,  i2_spi_data_width_t data_width,
                        i2_spi_clock_speed_t clk_speed,
                        i2_spi_mode_t spi_mode, i2_spi_first_bit_t first_bit,
                        uint8_t *rxbuf, int32_t size, uint32_t timeout)
{
  return i2_spi_txrx_raw( inst, data_width, clk_speed, spi_mode,
                          first_bit, NULL, rxbuf, size, timeout);
}

/**
 * @brief   SPI bus assert.
 * @details Asserts the CS pin of SPI instance.
 *
 * @param[in] *inst       SPI instance.
 * @param[in] timeout     RTOS wait for MUTEX timeout.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_cs_assert(i2_spi_inst_t *inst, uint32_t timeout)
{
  i2_spi_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  if (xSemaphoreTake(ctx->mutex, (TickType_t)timeout) == pdFALSE) {
    return I2_TIMEOUT;
  }
#endif /* ENABLE_RTOS_AWARE_HAL */

  /* assert CS */
  if ( i2_gpio_is_valid(&inst->CS) ) {
    i2_gpio_set(&inst->CS, I2_LOW);
  }

  return I2_SUCCESS;
}

/**
 * @brief   SPI bus deassert.
 * @details Desserts the CS pin of SPI instance.
 *
 * @param[in] *inst       SPI instance.
 * @return  Execution error code @ref I2_ERROR.
 */
i2_error i2_spi_cs_deassert(i2_spi_inst_t *inst)
{
  i2_spi_ctx_t *ctx;

  if ( !inst ) {
    return I2_INVALID_PARAM;
  }

  ctx = spi_inst_to_ctx(inst->ctx_name);
  if ( !ctx ) {
    return I2_INVALID_PARAM;
  }

  /* deassert CS */
  if ( i2_gpio_is_valid(&inst->CS) ) {
    i2_gpio_set(&inst->CS, I2_HIGH);
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx->mutex);
#endif /* ENABLE_RTOS_AWARE_HAL */

  return I2_SUCCESS;
}

#if defined ( I2_ENABLE_SPI1_CONTEXT )
/**
 * @brief   SPI1 interrupt Handler.
 * @details Generic interrupt handler for SPI1.
 *
 * @return  None.
 */
void SPI1_IRQHandler(void)
{
  HAL_SPI_IRQHandler(spi1);
}

/**
 * @brief   SPI1 DMA receive interrupt Handler.
 * @details DMA receive interrupt handler for SPI1.
 *
 * @return  None.
 */
void SPI1_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi1->hdmarx);
}

/**
 * @brief   SPI1 DMA transmit interrupt Handler.
 * @details DMA transmit interrupt handler for SPI1.
 *
 * @return  None.
 */
void SPI1_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi1->hdmatx);
}
#endif /* I2_ENABLE_SPI1_CONTEXT */

#if defined ( I2_ENABLE_SPI2_CONTEXT )
/**
 * @brief   SPI2 interrupt Handler.
 * @details Generic interrupt handler for SPI2.
 *
 * @return  None.
 */
void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(spi2);
}

/**
 * @brief   SPI2 DMA receive interrupt Handler.
 * @details DMA receive interrupt handler for SPI2.
 *
 * @return  None.
 */
void SPI2_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi2->hdmarx);
}

/**
 * @brief   SPI2 DMA transmit interrupt Handler.
 * @details DMA transmit interrupt handler for SPI2.
 *
 * @return  None.
 */
void SPI2_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi2->hdmatx);
}
#endif /* I2_ENABLE_SPI2_CONTEXT */

#if defined ( I2_ENABLE_SPI3_CONTEXT )
/**
 * @brief   SPI3 interrupt Handler.
 * @details Generic interrupt handler for SPI3.
 *
 * @return  None.
 */
void SPI3_IRQHandler(void)
{
  HAL_SPI_IRQHandler(spi3);
}

/**
 * @brief   SPI3 DMA receive interrupt Handler.
 * @details DMA receive interrupt handler for SPI3.
 *
 * @return  None.
 */
void SPI3_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi3->hdmarx);
}

/**
 * @brief   SPI3 DMA transmit interrupt Handler.
 * @details DMA transmit interrupt handler for SPI3.
 *
 * @return  None.
 */
void SPI3_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi3->hdmatx);
}
#endif /* I2_ENABLE_SPI3_CONTEXT */

/**
 * @brief   SPI completion callback.
 * @details System callback for SPI transmission and reception completion.
 *
 * @param[in] *hspi     SPI handler.
 * @return  None.
 */
static void spi_cplt_callback(SPI_HandleTypeDef *hspi)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  i2_spi_ctx_t *ctx = spi_handle_to_ctx(hspi);

  if (ctx) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    xSemaphoreGiveFromISR(ctx->sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
    ctx->status = I2_TRANSFER_DONE;
  }
}

/**
 * @brief   SPI DMA Transmit and receive compete callback.
 * @details System callback for DMA transmission and reception completion.
 *
 * @param[in] *hspi     SPI handler.
 * @return  None.
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_cplt_callback(hspi);
}

/**
 * @brief   SPI DMA Transmit compete callback.
 * @details System callback for DMA transmission completion.
 *
 * @param[in] *hspi     SPI handler.
 * @return  None.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_cplt_callback(hspi);
}

/**
 * @brief   SPI DMA receive compete callback.
 * @details System callback for DMA reception completion.
 *
 * @param[in] *hspi     SPI handler.
 * @return  None.
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_cplt_callback(hspi);
}

/**
 * @brief   SPI DMA error callback.
 * @details System callback for error during DMA operations on SPI bus.
 *
 * @param[in] *hspi     SPI handler.
 * @return  None.
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif /* ENABLE_RTOS_AWARE_HAL */
  i2_spi_ctx_t *ctx = spi_handle_to_ctx(hspi);

  if ( ctx ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    xSemaphoreGiveFromISR(ctx->sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif /* ENABLE_RTOS_AWARE_HAL */
    ctx->status = I2_TRANSFER_ERROR;
  }
}

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
