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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include <time.h>
#include <i2_stm32f4xx_hal_common.h>

#if defined( ENABLE_RTOS_AWARE_HAL )
#include <FreeRTOS.h>
#include <semphr.h>
#endif

/* Exported constants --------------------------------------------------------*/
/* Two alarm instance IDs */
typedef enum {
  RTC_ALARM_1 = 0,
  RTC_ALARM_2,
  MAX_NUM_RTC_ALARMS,
} rtc_alarm_t;

/*
 * An RTC alarm can either trigger off date of each month
 * or weekday of each week. The alarm can't be set with
 * year or month fields.
 */
typedef enum {
  RTC_ALARM_TYPE_NONE = 0,
  RTC_ALARM_TYPE_WEEKDAY,
  RTC_ALARM_TYPE_DATE,
  MAX_NUM_RTC_ALARM_TYPES,
} rtc_alarm_type_t;

i2_error i2_rtc_init(void);
i2_error i2_rtc_time_get(struct tm *time);
i2_error i2_rtc_time_set(struct tm *time);

/*
 * If the given alarm ID is not armed, the alarm_type will be
 * set to RTC_ALARM_TYPE_NONE and the alarm data meaningless.
 */
i2_error i2_rtc_alarm_get(rtc_alarm_t alarm_id,
                          rtc_alarm_type_t *alarm_type, struct tm *alarm);
/*
 * If the given alarm_id is already set/armed, it will overwrite the existing
 * alarm setting without any error.
 */
i2_error i2_rtc_alarm_set(rtc_alarm_t alarm_id,
                          rtc_alarm_type_t alarm_type, struct tm *alarm);

/*
 * This disables the alarm interrupt but the associated callback stays, if it exists.
 * To re-enable the alarm, please use rtc_alarm_set() to set it again to new value.
 */
i2_error i2_rtc_alarm_delete(rtc_alarm_t alarm_id);

i2_error i2_rtc_wakeup_set(uint32_t period);
i2_error i2_rtc_wakeup_get(uint32_t *period);
i2_error i2_rtc_wakeup_delete(void);

/*
 * Note the callback runs in the rtc interrupt context. Thus, if the callback
 * requires a heavy processing, sleep, delay, or mutex/semaphore, please have
 * the callback simply signal a task to perform the lower half work.
 */
i2_error i2_rtc_alarm_handler_register(rtc_alarm_t alarm_id,
                                       void (*cb)(void *arg), void *arg);
i2_error i2_rtc_alarm_handler_unregister(rtc_alarm_t alarm_id);

i2_error i2_rtc_wakeup_handler_register(void (*cb)(void *arg), void *arg);
i2_error i2_rtc_wakeup_handler_unregister(void);

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
