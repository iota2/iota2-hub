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
 * @file        i2_stm32f4xx_hal_rtc.h
 * @brief       RTC set-up and control.
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
#include <time.h>
#include <i2_stm32f4xx_hal_common.h>

#if defined( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

/* Exported constants --------------------------------------------------------*/
/**
 * @defgroup rtc_alarm_t RTC Alarm instances.
 * Two alarm instance IDs.
 *
 * @{
 */
/** @brief RTC available alarms */
typedef enum {
  RTC_ALARM_1 = 0,          /**< RTC Alarm 1                    */
  RTC_ALARM_2,              /**< RTC Alarm 2                    */
  MAX_NUM_RTC_ALARMS,       /**< Maximum RTC Alarm counts       */
} rtc_alarm_t;              /**< RTC Alarm instance             */
/** @} */ /* rtc_alarm_t */

/**
 * @defgroup rtc_alarm_type_t RTC Alarm type.
 * An RTC alarm can either trigger off date of each month
 * or weekday of each week. The alarm can't be set with
 * year or month fields.
 *
 * @{
 */
/** @brief RTC alarms options */
typedef enum {
  RTC_ALARM_TYPE_NONE = 0,  /**< Alarm type not configured      */
  RTC_ALARM_TYPE_WEEKDAY,   /**< Alarm set for weekday          */
  RTC_ALARM_TYPE_DATE,      /**< Alarm set for days of month    */
  MAX_NUM_RTC_ALARM_TYPES,  /**< Maximum number of alarm types  */
} rtc_alarm_type_t;         /**< RTC Alarm type                 */
/** @} */ /* rtc_alarm_type_t */

/* Public functions --------------------------------------------------------- */
i2_error i2_rtc_init(void);
i2_error i2_rtc_time_get(struct tm *time);
i2_error i2_rtc_time_set(struct tm *time);
i2_error i2_rtc_alarm_get(rtc_alarm_t alarm_id,
                          rtc_alarm_type_t *alarm_type, struct tm *alarm);
i2_error i2_rtc_alarm_set(rtc_alarm_t alarm_id,
                          rtc_alarm_type_t alarm_type, struct tm *alarm);
i2_error i2_rtc_alarm_delete(rtc_alarm_t alarm_id);
i2_error i2_rtc_wakeup_set(uint32_t period);
i2_error i2_rtc_wakeup_get(uint32_t *period);
i2_error i2_rtc_wakeup_delete(void);
i2_error i2_rtc_alarm_handler_register(rtc_alarm_t alarm_id,
                                       void (*cb)(void *arg), void *arg);
i2_error i2_rtc_alarm_handler_unregister(rtc_alarm_t alarm_id);
i2_error i2_rtc_wakeup_handler_register(void (*cb)(void *arg), void *arg);
i2_error i2_rtc_wakeup_handler_unregister(void);

/************************ (C) COPYRIGHT iota2 ***[i2]*****END OF FILE**********/
