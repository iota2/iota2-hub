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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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

    /* GPIOI Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    /* Configure PI8 and PI9 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

    for( ;; )
    {
        /* Task code goes here. */
      /* Set PI8 and PI9 */
      GPIOI->BSRRL = LED1_PIN | LED2_PIN;
      /* Reset PI8 and PI9 */
      GPIOI->BSRRH = LED1_PIN | LED2_PIN;

      /* Set PI8 and PI9 */
      GPIOI->BSRRL = LED1_PIN | LED2_PIN;
      /* Reset PI8 and PI9 */
      GPIOI->BSRRH = LED1_PIN | LED2_PIN;
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

  //Create user task
  HUB_statusHandle = xTaskCreate( HUB_taskUSER, "HUB",  HUB_taskStckDepthUSER,
      ( void * ) 1, HUB_taskPritorityUSER, &HUB_taskHandleUSER );
  if ( HUB_statusHandle != pdPASS ) {
    return 0;
  }


  //Start the scheduler
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

