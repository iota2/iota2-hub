/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 2 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __GNUC__
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

/**
 * @defgroup i2_FreeRTOS_config FreeRTOS Configurations.
 * Generic configurations for FreeRTOS integration.
 *
 * @{
 */
#define configUSE_PREEMPTION              1   /**< Define to enable Preemption  */
#define configUSE_IDLE_HOOK               0   /**< Define to enable Idle Hook   */
#define configUSE_TICK_HOOK               0   /**< Define to enable Tick Hook   */
#define configCPU_CLOCK_HZ                ( SystemCoreClock )     /**< CPU CLock source */
#define configTICK_RATE_HZ                ( ( TickType_t ) 1000 ) /**< CPU Tick rate  */
#define configMIN_PRIORITIES              ( tskIDLE_PRIORITY )    /**< Minimum defined priority */
#define configMAX_PRIORITIES              ( 5 )                   /**< Maximum defined task priority */
#define configMINIMAL_STACK_SIZE          ( ( unsigned short ) 128 )    /**< Minimum defined Stack size */
#define configTOTAL_HEAP_SIZE             ( ( size_t ) ( 75 * 1024 ) )  /**< Total Heap allocated to RTOS */
#define configMAX_TASK_NAME_LEN           10  /**< Maximum length of task name */
#define configUSE_TRACE_FACILITY          0   /**< Define to enable trace facility  */
#define configUSE_16_BIT_TICKS            0   /**< Define to enable 16bit ticks   */
#define configIDLE_SHOULD_YIELD           1   /**< Define if Idle yeild need to be checked */
#define configUSE_MUTEXES                 1   /**< Define to enable use of Mutex  */
#define configQUEUE_REGISTRY_SIZE         8   /**< Maximum queue registry size    */
#define configCHECK_FOR_STACK_OVERFLOW    0   /**< Define to enable check for stack overflow */
#define configUSE_RECURSIVE_MUTEXES       0   /**< Define to enable recursive mutexes */
#define configUSE_MALLOC_FAILED_HOOK      0   /**< Define to use malloc failed hook */
#define configUSE_APPLICATION_TASK_TAG    0   /**< Define to enable tagging for application task */
#define configUSE_COUNTING_SEMAPHORES     1   /**< Define to enable couting semaphores */
#define configGENERATE_RUN_TIME_STATS     0   /**< Define to generate run time stats */
/** @} */ /* i2_FreeRTOS_config */

/**
 * @defgroup i2_FreeRTOS_Co_Routines FreeRTOS co-routine definitions.
 * Generic configurations for FreeRTOS Co-Routines.
 *
 * @{
 */
#define configUSE_CO_ROUTINES 	          0   /**< Define to enable Co-routines */
#define configMAX_CO_ROUTINE_PRIORITIES   2   /**< Maximum properties for Co-routines */
/** @} */ /* i2_FreeRTOS_CO_Routines */

/**
 * @defgroup i2_FreeRTOS_Timers FreeRTOS software timer definitions.
 * Generic configurations for FreeRTOS Timers.
 *
 * @{
 */
#define configUSE_TIMERS                  1   /**< Define to enable Timers    */
#define configTIMER_TASK_PRIORITY         2   /**< Priority for Timer tasks   */
#define configTIMER_QUEUE_LENGTH          10  /**< Queue length for Timers    */
#define configTIMER_TASK_STACK_DEPTH      ( configMINIMAL_STACK_SIZE * 2 )  /**< Timer tasks stack depth */
/** @} */ /* i2_FreeRTOS_Timers */

/**
 * @defgroup i2_FreeRTOS_Tasks FreeRTOS additional APIs.
 * Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.
 *
 * @{
 */
#define INCLUDE_vTaskPrioritySet          1   /**< Enable vTaskPrioritySet API  */
#define INCLUDE_uxTaskPriorityGet         1   /**< Enable uxTaskPriorityGet API */
#define INCLUDE_vTaskDelete               0   /**< Disable vTaskDelete API      */
#define INCLUDE_vTaskCleanUpResources     1   /**< Enable vTaskCleanUpResources API */
#define INCLUDE_vTaskSuspend              1   /**< Enable vTaskSuspend API      */
#define INCLUDE_vTaskDelayUntil           1   /**< Enable vTaskDelayUntil API   */
#define INCLUDE_vTaskDelay                1   /**< Enable vTaskDelay API        */
/** @} */ /* i2_FreeRTOS_Tasks */

/**
 * @defgroup i2_FreeRTOS_CortexM FreeRTOS Cortex-M specific definitions.
 * Definitions related to Cortex M core.
 *
 * @{
 */
#ifdef __NVIC_PRIO_BITS
/** @brief __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
#define configPRIO_BITS                   __NVIC_PRIO_BITS
#else
#define configPRIO_BITS                   4        /**<  15 priority levels */
#endif
/** @} */ /* i2_FreeRTOS_CortexM */

/** @brief  The lowest interrupt priority that can be used in a call to a
 *          "set priority" function.
 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       0xf

/** @brief  The highest interrupt priority that can be used by any interrupt
 *          service routine that makes calls to interrupt safe FreeRTOS API
 *          functions.  DO NOT CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM
 *          ANY INTERRUPT THAT HAS A HIGHER PRIORITY THAN THIS!
 *          (higher priorities are lower numeric values.
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5

/** @brief  Interrupt priorities used by the kernel port layer itself.
 *          These are generic to all Cortex-M ports, and do not rely on any
 *          particular library functions.
 */
#define configKERNEL_INTERRUPT_PRIORITY               ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/** @brief !!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!
 *          See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html.
 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY          ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/** @brief  Normal assert() semantics without relying on the provision of an
 *          assert.h header file.
 */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/** @brief
 */
/**
 * @defgroup i2_FreeRTOS_IRQn FreeRTOS interrupt Handler.
 * Mapping for FreeRTOS port interrupt handlers to their CMSIS standard names.
 *
 * @{
 */
#define vPortSVCHandler       SVC_Handler     /**< SVCall exception handler   */
#define xPortPendSVHandler    PendSV_Handler  /**< Pending SV exeption handler*/
#define xPortSysTickHandler   SysTick_Handler /**< Systic execption handler   */
/** @} */ /* i2_FreeRTOS_IRQn */

#endif /* FREERTOS_CONFIG_H */
