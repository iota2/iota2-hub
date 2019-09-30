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
#include "i2_stm32f4xx_hal_gpio.h"

#include <string.h>
#if defined ( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif /* ENABLE_RTOS_AWARE_HAL */

/* Private defines -----------------------------------------------------------*/
#define MAX_NUM_IO                        ( 144 )
#define NUM_GPIO_PORTS                    ( 9 )
#define NUM_GPIO_PER_PORT                 ( 16 )
#define MAX_NUM_GPIO_INTERRUPTS           ( 16 )
#define GPIO_EXTI_PREEMPTION_PRIORITY     ( 5 )
#define GPIO_EXTI_SUB_PRIORITY            ( 1 )

/* Private variables ---------------------------------------------------------*/
static bool initialized = false;
static i2_gpio_inst_t *gpio_inst[MAX_NUM_IO] = {0};
static i2_handler_t gpio_isr_inst[MAX_NUM_GPIO_INTERRUPTS] = {{0}};

/* Information on each GPIO port */
typedef struct {
  const char        *name;
  GPIO_TypeDef      *gpio_port;
  int32_t           refcount;
  bool              clk_on;
#if defined( ENABLE_RTOS_AWARE_HAL )
  SemaphoreHandle_t mutex;
#endif /* ENABLE_RTOS_AWARE_HAL */
} i2_gpio_ctx_t;

static i2_gpio_ctx_t GPIO_PORTS[NUM_GPIO_PORTS] = {
  { "PORT_A", GPIOA, 0, false },
  { "PORT_B", GPIOB, 0, false },
  { "PORT_C", GPIOC, 0, false },
  { "PORT_D", GPIOD, 0, false },
  { "PORT_E", GPIOE, 0, false },
  { "PORT_F", GPIOF, 0, false },
  { "PORT_G", GPIOG, 0, false },
  { "PORT_H", GPIOH, 0, false },
  { "PORT_I", GPIOI, 0, false },
};

static int16_t GPIO_PINS[NUM_GPIO_PER_PORT] = {
  GPIO_PIN_0,   GPIO_PIN_1,   GPIO_PIN_2,   GPIO_PIN_3,
  GPIO_PIN_4,   GPIO_PIN_5,   GPIO_PIN_6,   GPIO_PIN_7,
  GPIO_PIN_8,   GPIO_PIN_9,   GPIO_PIN_10,  GPIO_PIN_11,
  GPIO_PIN_12,  GPIO_PIN_13,  GPIO_PIN_14,  GPIO_PIN_15,
};

/* Private functions ---------------------------------------------------------*/
static int32_t get_port_index(GPIO_TypeDef *gpio_port)
{
  int32_t i;

  for ( i = 0; i < NUM_GPIO_PORTS; i++ ) {
    if ( GPIO_PORTS[i].gpio_port == gpio_port ) {
      break;
    }
  }

  return i;
}

static int32_t get_gpio_index(uint16_t gpio)
{
  int32_t i;

  for ( i = 0; i < NUM_GPIO_PER_PORT; i++ ) {
    if ( GPIO_PINS[i] == gpio ) {
      break;
    }
  }

  return i;
}

static int32_t gpio_refcount_get(GPIO_TypeDef *gpio_port)
{
  int32_t index = get_port_index(gpio_port);

  if ( index >= NUM_GPIO_PORTS ) {
    return -1;
  }
  return GPIO_PORTS[index].refcount;
}

static void gpio_refcount_up(GPIO_TypeDef *gpio_port)
{
  int32_t index = get_port_index(gpio_port);

  if ( index >= NUM_GPIO_PORTS ) {
    return;
  }
  GPIO_PORTS[index].refcount++;

  return;
}

static void gpio_refcount_down(GPIO_TypeDef *gpio_port)
{
  int32_t index = get_port_index(gpio_port);

  if ( index >= NUM_GPIO_PORTS ) {
    return;
  }
  GPIO_PORTS[index].refcount--;

  return;
}

static void gpio_mutex_acquire(GPIO_TypeDef *gpio_port)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  int32_t index = get_port_index(gpio_port);

  if (index >= NUM_GPIO_PORTS) {
      return;
  }

  xSemaphoreTake(GPIO_PORTS[index].mutex, portMAX_DELAY);
#else
  UNUSED_PARAMETER( gpio_port );
#endif /* ENABLE_RTOS_AWARE_HAL */
}

static void gpio_mutex_release(GPIO_TypeDef *gpio_port)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  int32_t index = get_port_index(gpio_port);

  if (index >= NUM_GPIO_PORTS) {
      return;
  }

  xSemaphoreGive(GPIO_PORTS[index].mutex);
#else
  UNUSED_PARAMETER( gpio_port );
#endif /* ENABLE_RTOS_AWARE_HAL */
}

static void gpio_clock_enable(GPIO_TypeDef *gpio_port)
{
  int32_t index = get_port_index(gpio_port);

  if ( index >= NUM_GPIO_PORTS ) {
    return;
  }

  if ( gpio_port == GPIOA ) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  } else if ( gpio_port == GPIOB ) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  } else if ( gpio_port == GPIOC ) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  } else if ( gpio_port == GPIOD ) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  } else if ( gpio_port == GPIOE ) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  } else if ( gpio_port == GPIOF ) {
    __HAL_RCC_GPIOF_CLK_ENABLE();
  } else if ( gpio_port == GPIOG ) {
    __HAL_RCC_GPIOG_CLK_ENABLE();
  } else if ( gpio_port == GPIOH ) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
  } else if ( gpio_port == GPIOI ) {
    __HAL_RCC_GPIOI_CLK_ENABLE();
  }
  GPIO_PORTS[index].clk_on = true;
}

static void gpio_clock_disable(GPIO_TypeDef *gpio_port)
{
  int32_t index = get_port_index(gpio_port);

  if (index >= NUM_GPIO_PORTS) {
    return;
  }

  if (gpio_port == GPIOA) {
    __HAL_RCC_GPIOA_CLK_DISABLE();
  } else if (gpio_port == GPIOB) {
    __HAL_RCC_GPIOB_CLK_DISABLE();
  } else if (gpio_port == GPIOC) {
    __HAL_RCC_GPIOC_CLK_DISABLE();
  } else if (gpio_port == GPIOD) {
    __HAL_RCC_GPIOD_CLK_DISABLE();
  } else if (gpio_port == GPIOE) {
    __HAL_RCC_GPIOE_CLK_DISABLE();
  } else if (gpio_port == GPIOF) {
    __HAL_RCC_GPIOF_CLK_DISABLE();
  } else if (gpio_port == GPIOG) {
    __HAL_RCC_GPIOG_CLK_DISABLE();
  } else if (gpio_port == GPIOH) {
    __HAL_RCC_GPIOH_CLK_DISABLE();
  } else if (gpio_port == GPIOI) {
    __HAL_RCC_GPIOI_CLK_DISABLE();
  }
  GPIO_PORTS[index].clk_on = false;
}

static bool is_gpio_available(i2_gpio_inst_t *inst)
{
  int32_t i;

  for ( i = 0; i < MAX_NUM_IO; i++ ) {
    if ( gpio_inst[i] &&
         gpio_inst[i]->gpio_port == inst->gpio_port &&
         gpio_inst[i]->gpio == inst->gpio ) {
      return false;
    }
  }

  return true;
}

static void gpio_activate(i2_gpio_inst_t *inst)
{
  GPIO_InitTypeDef config;

  if ( inst->active ) {
    return;
  }

  if ( gpio_refcount_get(inst->gpio_port) == 0 ) {
    gpio_clock_enable(inst->gpio_port);
  }

  config.Pin        = inst->gpio;
  config.Pull       = inst->pull;
  config.Mode       = inst->mode;
  config.Alternate  = inst->alt;
  config.Speed      = inst->speed;
  HAL_GPIO_Init(inst->gpio_port, &config);

  gpio_refcount_up(inst->gpio_port);
  inst->active = true;
}

static void gpio_deactivate(i2_gpio_inst_t *inst)
{
  if ( !inst->active ) {
    return;
  }

  gpio_refcount_down(inst->gpio_port);
  HAL_GPIO_DeInit(inst->gpio_port, inst->gpio);

  if ( gpio_refcount_get(inst->gpio_port) == 0 ) {
    gpio_clock_disable(inst->gpio_port);
  }

  inst->active = false;
}

/* Public functions --------------------------------------------------------- */
void i2_gpio_init(void)
{
  if ( false == initialized ) {
    int32_t i;

    /* Start with analog input, no pull */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_All);
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_All);

    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    __HAL_RCC_GPIOI_CLK_DISABLE();

#if defined ( ENABLE_RTOS_AWARE_HAL )
    for (i = 0; i < NUM_GPIO_PORTS; i++) {
      GPIO_PORTS[i].mutex = xSemaphoreCreateMutex();
    }
#endif /* ENABLE_RTOS_AWARE_HAL */

    initialized = true;
  }
}

i2_gpio_inst_t* i2_gpio_ctx_get(char *name)
{
  int32_t i;

  for ( i = 0; i < MAX_NUM_IO; i++ ) {
    if ( gpio_inst[i] && !strcmp(gpio_inst[i]->name, name) ) {
      return gpio_inst[i];
    }
  }

  return NULL;
}

bool i2_gpio_is_valid(i2_gpio_inst_t *inst)
{
  if ( !inst->gpio_port || !inst->gpio ) {
    return false;
  }

  return true;
}

void i2_gpio_release(i2_gpio_inst_t *inst)
{
  int32_t i;

  if ( NULL == inst ) {
    return;
  }

  gpio_mutex_acquire(inst->gpio_port);

  if ( true == is_gpio_available(inst) ) {
    goto err;
  }

  gpio_deactivate(inst);

  for ( i = 0; i < MAX_NUM_IO; i++ ) {
    if ( gpio_inst[i] == inst ) {
      gpio_inst[i] = 0;
      break;
    }
  }

err:
  gpio_mutex_release(inst->gpio_port);
}

void i2_gpio_config(i2_gpio_inst_t *inst, uint32_t mode, uint32_t pull)
{
  i2_gpio_config_alt(inst, mode, pull, 0);
}

void i2_gpio_config_out(i2_gpio_inst_t *inst, bool val)
{
  i2_gpio_config(inst, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

  HAL_GPIO_WritePin(inst->gpio_port, (uint16_t)inst->gpio,
                    (val ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

void i2_gpio_config_in(i2_gpio_inst_t *inst, uint32_t pull)
{
  i2_gpio_config(inst, GPIO_MODE_INPUT, pull);
}

void i2_gpio_config_alt(i2_gpio_inst_t *inst, uint32_t mode,
                     uint32_t pull, uint32_t alt)
{
  int32_t i;

  if ( NULL == inst ) {
    return;
  }

  gpio_mutex_acquire(inst->gpio_port);

  if ( !is_gpio_available( inst) ) {
    i2_assert(0);
    goto err;
  }

  inst->speed  = GPIO_SPEED_FREQ_VERY_HIGH;
  inst->pull   = pull;
  inst->mode   = mode;
  inst->alt    = alt;
  gpio_activate( inst );

  for ( i = 0; i < MAX_NUM_IO; i++ ) {
    if ( !gpio_inst[i] ) {
      gpio_inst[i] = inst;
      break;
    }
  }

err:
  gpio_mutex_release(inst->gpio_port);
}

bool i2_gpio_get(i2_gpio_inst_t *inst)
{
  if (HAL_GPIO_ReadPin(inst->gpio_port, (uint16_t)inst->gpio) == GPIO_PIN_SET) {
      return true;
  }

  return false;
}

void i2_gpio_set(i2_gpio_inst_t *inst, bool val)
{
  HAL_GPIO_WritePin(inst->gpio_port, (uint16_t)inst->gpio,
                    (val ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

void i2_gpio_toggle(i2_gpio_inst_t *inst)
{
  HAL_GPIO_TogglePin(inst->gpio_port, (uint16_t)inst->gpio);
}

i2_error i2_gpio_config_interrupt(i2_gpio_inst_t *inst, uint32_t mode,
                               uint32_t pull, void (*cb)(void *arg), void *arg)
{
  int32_t index;
  IRQn_Type irqn;

  index = get_gpio_index(inst->gpio);

  /* Invalid gpio */
  if ( index >= NUM_GPIO_PER_PORT ) {
    return I2_INVALID_PARAM;
  }

  /* Interrupt already registered for this line */
  if ( gpio_isr_inst[index].cb ) {
    return I2_NOT_AVAILABLE;
  }

  if ( index == 0 ) {
    irqn = EXTI0_IRQn;
  } else if ( index == 1 ) {
    irqn = EXTI1_IRQn;
  } else if ( index == 2 ) {
    irqn = EXTI2_IRQn;
  } else if ( index == 3 ) {
    irqn = EXTI3_IRQn;
  } else if ( index == 4 ) {
    irqn = EXTI4_IRQn;
  } else if ( index >= 5 && index <= 9 ) {
    irqn = EXTI9_5_IRQn;
  } else if ( index >= 10 && index <= 15 ) {
    irqn = EXTI15_10_IRQn;
  } else {
    return I2_INVALID_PARAM;
  }
  i2_gpio_config(inst, mode, pull);

  HAL_NVIC_SetPriority(irqn,
                       GPIO_EXTI_PREEMPTION_PRIORITY,
                       GPIO_EXTI_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(irqn);

  gpio_isr_inst[index].cb = cb;
  gpio_isr_inst[index].arg = arg;

  return I2_SUCCESS;
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
  * @brief EXTI line detection callbacks
  * @param gpio: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t gpio)
{
  int32_t index = get_gpio_index(gpio);

  /* Invalid gpio - shouldn't happen */
  if (index >= NUM_GPIO_PER_PORT) {
    return;
  }

  /* Call the registered isr */
  if (gpio_isr_inst[index].cb) {
    gpio_isr_inst[index].cb(gpio_isr_inst[index].arg);
  }

  return;
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
