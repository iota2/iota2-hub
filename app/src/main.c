/**
 * @author      iota square <i2>
 * @date        07-09-2019
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
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static i2_uart_inst_t uart_console = {
  "console", "USART1",
};

static i2_spi_inst_t ext_flash = {
    "extflash", "SPI1", { "extflash_CS", GPIOG, GPIO_PIN_15 }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  User task
  * @param  pvParameters : Imported parameters to be used in task
  * @retval None
  */
void HUB_taskUSER( void * pvParameters )
{
  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below. */
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

  for( ;; ) {
    /* Task code goes here. */
    i2_led_toggle(LED1);
    /* Insert delay 100 ms */
    vTaskDelay(100);

    i2_led_toggle(LED2);
    /* Insert delay 100 ms */
    vTaskDelay(100);
  }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  BaseType_t    HUB_statusHandle;
  TaskHandle_t  HUB_taskHandleUSER = NULL;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization */
  HAL_Init();

  /* Interface and peripherals initializations */
  i2_hse_lse_clock_config();
  i2_rtc_init();
  i2_led_init();
  i2_uart_init( &uart_console );
  i2_spi_init( &ext_flash );
  ssd1306_init(SSD1306_CMD_SWITCH_CAP_VCC);

  /* Create user task */
  HUB_statusHandle = xTaskCreate( HUB_taskUSER, "HUB",  HUB_taskStckDepthUSER,
      ( void * ) 1, HUB_taskPritorityUSER, &HUB_taskHandleUSER );
  if ( HUB_statusHandle != pdPASS ) {
    return 0;
  }

  /* Start the scheduler */
  vTaskStartScheduler();

  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/

