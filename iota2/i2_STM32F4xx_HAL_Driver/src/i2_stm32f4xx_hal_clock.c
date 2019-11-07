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
 * @file        i2_stm32f4xx_hal_clock.c
 * @brief       HAL clock set-up.
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
#include "i2_stm32f4xx_hal_clock.h"

/* Private variables ---------------------------------------------------------*/
static bool lse_on = false;   /**< LSE on/off state */

/* Private functions ---------------------------------------------------------*/
/**
 * @brief   Common clock configurations.
 * @details Initializes Clock PLL.
 *
 * @return  None.
 */
static void common_clock_config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 );
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if ( HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001) {
  /* Enable the Flash prefetch */
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }
}

/* Public functions ----------------------------------------------------------*/
/**
 * @brief   Check LSE state.
 * @details Checks of state of LSE is enabled or not.
 *
 * @return  True if LSE is on and used for RTC.
 */
bool i2_is_lse_on(void)
{
  return lse_on;
}

/**
 * @brief   Use external clocks.
 * @details Configures HSE to drives PLL and LSE to drives RTC.
 *
 * @return  None.
 */
void i2_hse_lse_clock_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* PWR clock is needed for HAL_PWR APIs to work */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE |
                                      RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState        = RCC_HSI_OFF;
  RCC_OscInitStruct.LSEState        = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState        = RCC_LSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM        = 25;
  RCC_OscInitStruct.PLL.PLLN        = 336;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }

  lse_on = true;

  common_clock_config();
}

/**
 * @brief   Use internal clocks.
 * @details Configure HSI to drives PLL and LSI to drives RTC.
 *
 * @return  None.
 */
void i2_hsi_lsi_clock_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* PWR clock is needed for HAL_PWR APIs to work */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI |
                                      RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState        = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState        = RCC_HSI_ON;
  RCC_OscInitStruct.LSEState        = RCC_LSE_OFF;
  RCC_OscInitStruct.LSIState        = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM        = 16;
  RCC_OscInitStruct.PLL.PLLN        = 336;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }

  lse_on = false;

  common_clock_config();
}

/**
 * @brief   Use external / internal clocks.
 * @details Configure HSE to drives PLL and LSI to drives RTC.
 *
 * @return  None.
 */
void i2_hse_lsi_clock_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* PWR clock is needed for HAL_PWR APIs to work */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE |
                                      RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState        = RCC_HSI_OFF;
  RCC_OscInitStruct.LSEState        = RCC_LSE_OFF;
  RCC_OscInitStruct.LSIState        = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM        = 25;
  RCC_OscInitStruct.PLL.PLLN        = 336;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }

  lse_on = false;

  common_clock_config();
}

/**
 * @brief   Use internal / external clocks.
 * @details Configure HSI to drives PLL and  LSE to drives RTC.
 *
 * @return  None.
 */
void i2_hsi_lse_clock_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* PWR clock is needed for HAL_PWR APIs to work */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI |
                                      RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState        = RCC_HSI_ON;
  RCC_OscInitStruct.LSEState        = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState        = RCC_LSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM        = 16;
  RCC_OscInitStruct.PLL.PLLN        = 336;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while (1);
  }

  lse_on = true;

  common_clock_config();
}

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
