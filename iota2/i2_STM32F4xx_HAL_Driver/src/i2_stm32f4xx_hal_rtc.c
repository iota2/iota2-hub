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
#include "i2_stm32f4xx_hal_rtc.h"
#include "i2_stm32f4xx_hal_clock.h"

/* Private macro -------------------------------------------------------------*/
#define RTC_ASYNCH_PREDIV               ( 0x7F )
#define RTC_SYNCH_PREDIV                ( 0xF9 )     /* (32Khz / 128) - 1 */
#define RTC_ALARM_A_STATUS_REG          ( RTC_BKP_DR0 )
#define RTC_ALARM_B_STATUS_REG          ( RTC_BKP_DR1 )
#define RTC_WAKEUP_CLK_SRC_REG          ( RTC_BKP_DR2 )
#define RTC_PREEMPTION_PRIORITY         ( 5 )
#define RTC_SUB_PRIORITY                ( 1 )

/* Private variables ---------------------------------------------------------*/
/* This defines the attributes of RTC hardware context. */
typedef struct {
  const char *controller_name;
  bool initialized;
  RTC_TypeDef *baseaddr;
  RTC_HandleTypeDef rtc;
  i2_handler_t alarm[MAX_NUM_RTC_ALARMS];
  i2_handler_t wakeup;
#if defined ( ENABLE_RTOS_AWARE_HAL )
  SemaphoreHandle_t mutex;
#endif
} rtc_ctx;

static rtc_ctx ctx = {
  "RTC", false, RTC,
};

static RTC_HandleTypeDef *rtc = NULL;

/* Private functions ---------------------------------------------------------*/
static void rtc_alarm_status_set(RTC_HandleTypeDef *handle,
                                  rtc_alarm_t alarm_id, bool arm)
{
  if (alarm_id == RTC_ALARM_1) {
    HAL_RTCEx_BKUPWrite(handle, RTC_ALARM_A_STATUS_REG, arm);
  } else if (alarm_id == RTC_ALARM_2) {
    HAL_RTCEx_BKUPWrite(handle, RTC_ALARM_B_STATUS_REG, arm);
  }
}

static bool rtc_alarm_status_get(RTC_HandleTypeDef *handle, rtc_alarm_t alarm_id)
{
  uint32_t val = 0;

  if (alarm_id == RTC_ALARM_1) {
    val = HAL_RTCEx_BKUPRead(handle, RTC_ALARM_A_STATUS_REG);
  } else if (alarm_id == RTC_ALARM_2) {
    val = HAL_RTCEx_BKUPRead(handle, RTC_ALARM_B_STATUS_REG);
  }
  return val & 0x1;
}

/* Public functions --------------------------------------------------------- */
i2_error i2_rtc_init(void)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  RTC_TypeDef *base;
  int32_t i;

  /* Prevent re-init */
  if ( ctx.initialized ) {
    return I2_NOT_AVAILABLE;
  }

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  /* Get the RTC controller base address */
  base = ctx.baseaddr;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  /* Create mutex. */
  ctx.mutex = xSemaphoreCreateMutex();
#endif

  if ( base == RTC ) {
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    HAL_PWR_EnableBkUpAccess();

    PeriphClkInitStruct.PeriphClockSelection  = RCC_PERIPHCLK_RTC;
    if ( i2_is_lse_on() ) {
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    } else {
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    }

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
      return I2_FAILURE;
    }

    __HAL_RCC_RTC_ENABLE();
    rtc = handle;
  } else {
    return I2_INVALID_PARAM;
  }

  HAL_NVIC_SetPriority(RTC_Alarm_IRQn,
                        RTC_PREEMPTION_PRIORITY,
                        RTC_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, RTC_PREEMPTION_PRIORITY, RTC_SUB_PRIORITY);
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

  handle->Instance = base;
  handle->Init.HourFormat = RTC_HOURFORMAT_24;
  if ( i2_is_lse_on() ) {
    handle->Init.AsynchPrediv = 127;
    handle->Init.SynchPrediv = 255;
  } else {
    handle->Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    handle->Init.SynchPrediv = RTC_SYNCH_PREDIV;
  }
  handle->Init.OutPut = RTC_OUTPUT_DISABLE;
  handle->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  handle->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  /* Initialize RTC with default sync prediv value */
  if (HAL_RTC_Init(handle) != HAL_OK) {
    return I2_FAILURE;
  }

  for (i = 0; i < MAX_NUM_RTC_ALARMS; i++) {
    ctx.alarm[i].cb = NULL;
    ctx.alarm[i].arg = NULL;
  }
  ctx.wakeup.cb = NULL;
  ctx.wakeup.arg = NULL;

  ctx.initialized = true;

  return retval;
}

i2_error i2_rtc_time_get(struct tm *time)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  RTC_DateTypeDef _date;
  RTC_TimeTypeDef _time;

  if ( !ctx.initialized || !time ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  if (HAL_RTC_GetTime(handle, &_time, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
    goto err;
  }
  time->tm_hour = _time.Hours;
  time->tm_min  = _time.Minutes;
  time->tm_sec  = _time.Seconds;
  time->tm_isdst = -1;

  /* No DST info. Do not use DST */
  if (_time.DayLightSaving == RTC_DAYLIGHTSAVING_NONE) {
    time->tm_isdst = -1;
  } else if (_time.DayLightSaving == RTC_DAYLIGHTSAVING_SUB1H) {
    time->tm_isdst = 0;
  } else {
    time->tm_isdst = 1;
  }

  if (HAL_RTC_GetDate(handle, &_date, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
    goto err;
  }
  time->tm_year = _date.Year + 100;
  time->tm_mon  = _date.Month - 1;
  time->tm_mday = _date.Date;
  time->tm_wday = (_date.WeekDay == 7) ? 0 : _date.WeekDay;

err:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_time_set(struct tm *time)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  RTC_DateTypeDef _date;
  RTC_TimeTypeDef _time;

  if ( !ctx.initialized || !time ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  _date.Year    = time->tm_year - 100;
  _date.Month   = time->tm_mon + 1;
  _date.Date    = time->tm_mday;
  _date.WeekDay = (!time->tm_wday ? 7 : time->tm_wday);

  if (HAL_RTC_SetDate(handle, &_date, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
    goto err;
  }

  _time.Hours          = time->tm_hour;
  _time.Minutes        = time->tm_min;
  _time.Seconds        = time->tm_sec;
  _time.SubSeconds     = 0x00;
  _time.TimeFormat     = (time->tm_hour < 12) ? RTC_HOURFORMAT12_AM : RTC_HOURFORMAT12_PM;

  _time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  if (time->tm_isdst < 0) {
    _time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  } else if (!time->tm_isdst) {
    _time.DayLightSaving = RTC_DAYLIGHTSAVING_SUB1H;
  } else {
    _time.DayLightSaving = RTC_DAYLIGHTSAVING_ADD1H;
  }

  _time.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(handle, &_time, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
  }

err:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_alarm_get(rtc_alarm_t alarm_id, rtc_alarm_type_t *alarm_type,
                          struct tm *alarm)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  RTC_AlarmTypeDef _alarm;

  if ( !ctx.initialized || !alarm || !alarm_type ) {
    return I2_INVALID_PARAM;
  }

  if ( (alarm_id < 0) || (alarm_id >= MAX_NUM_RTC_ALARMS) ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  /* Choose the alarm A or B */
  if (alarm_id == RTC_ALARM_1) {
    _alarm.Alarm = RTC_ALARM_A;
  } else {
    _alarm.Alarm = RTC_ALARM_B;
  }

  if (HAL_RTC_GetAlarm(handle, &_alarm, _alarm.Alarm, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
    goto out;
  }

  /* Get the hour, min, and sec */
  alarm->tm_hour = _alarm.AlarmTime.Hours;
  alarm->tm_min  = _alarm.AlarmTime.Minutes;
  alarm->tm_sec  = _alarm.AlarmTime.Seconds;
  alarm->tm_isdst = -1;

  /* RTC alarm does not match year or month */
  alarm->tm_year = 0;
  alarm->tm_mon  = 0;

  /* Check if it has been armed prior. If not, nothing is setup  */
  if (!rtc_alarm_status_get(handle, alarm_id)) {
    *alarm_type = RTC_ALARM_TYPE_NONE;
  } else if (_alarm.AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_WEEKDAY) {
    /* Alarm based on the week day */
    *alarm_type = RTC_ALARM_TYPE_WEEKDAY;
    alarm->tm_wday = (_alarm.AlarmDateWeekDay == 7) ? 0 : _alarm.AlarmDateWeekDay;
  } else if (_alarm.AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE) {
    /* Alarm based on date */
    *alarm_type = RTC_ALARM_TYPE_DATE;
    alarm->tm_mday = _alarm.AlarmDateWeekDay;
  }

out:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_alarm_set(rtc_alarm_t alarm_id, rtc_alarm_type_t alarm_type,
                           struct tm *alarm)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  RTC_AlarmTypeDef _alarm;

  if (!ctx.initialized || !alarm) {
    return I2_INVALID_PARAM;
  }

  if ( (alarm_id < 0) || (alarm_id >= MAX_NUM_RTC_ALARMS) ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Set the alarm_type to a valid value */
  if ((alarm_type != RTC_ALARM_TYPE_WEEKDAY) &&
      (alarm_type != RTC_ALARM_TYPE_DATE)) {
    retval = I2_INVALID_PARAM;
    goto out;
  }

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  /* Choose the alarm A or B */
  if ( alarm_id == RTC_ALARM_1 ) {
    _alarm.Alarm = RTC_ALARM_A;
  } else {
    _alarm.Alarm = RTC_ALARM_B;
  }
  /* Do not mask out anything. We will match day/date, hour, min, and sec */
  _alarm.AlarmMask = RTC_ALARMMASK_NONE;

  /* Depending on alarm type, we can match day or date */
  if (alarm_type == RTC_ALARM_TYPE_WEEKDAY) {
    _alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    /* Set the weekday */
    _alarm.AlarmDateWeekDay = (!alarm->tm_wday ? 7 : alarm->tm_wday);
  } else if (alarm_type == RTC_ALARM_TYPE_DATE) {
    _alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    /* Set date */
    _alarm.AlarmDateWeekDay = alarm->tm_mday;
  } else {
    retval = I2_INVALID_PARAM;
    goto out;
  }

  /* Don't care about matching the alarm at sub-second level */
  _alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;

  /* Set the hour, min, and sec */
  _alarm.AlarmTime.TimeFormat = (alarm->tm_hour < 12) ? RTC_HOURFORMAT12_AM : RTC_HOURFORMAT12_PM;
  _alarm.AlarmTime.Hours = alarm->tm_hour;
  _alarm.AlarmTime.Minutes = alarm->tm_min;
  _alarm.AlarmTime.Seconds = alarm->tm_sec;
  _alarm.AlarmTime.SubSeconds = 0;

  if (HAL_RTC_SetAlarm_IT(handle, &_alarm, RTC_FORMAT_BIN) != HAL_OK) {
    retval = I2_FAILURE;
  } else {
    rtc_alarm_status_set(handle, alarm_id, true);
  }

out:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_alarm_delete(rtc_alarm_t alarm_id)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  uint32_t id;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

  if ( (alarm_id < 0) || (alarm_id >= MAX_NUM_RTC_ALARMS) ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  if (alarm_id == RTC_ALARM_1) {
    id = RTC_ALARM_A;
  } else {
    id = RTC_ALARM_B;
  }

  if (HAL_RTC_DeactivateAlarm(handle, id) != HAL_OK) {
    retval = I2_FAILURE;
  } else {
    rtc_alarm_status_set(handle, alarm_id, false);
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

/*
 * @brief  Set periodical wake up timer
 * @param  period: Wake up counter
 * @param  clock_source: Wake up clock
 *
 *  clock source can be any one of the following value.
 *  RTC_WAKEUPCLOCK_RTCCLK_DIV16
 *  RTC_WAKEUPCLOCK_RTCCLK_DIV8
 *  RTC_WAKEUPCLOCK_RTCCLK_DIV4
 *  RTC_WAKEUPCLOCK_RTCCLK_DIV2
 *  RTC_WAKEUPCLOCK_CK_SPRE_16BITS  : clock 1Hz
 *  RTC_WAKEUPCLOCK_CK_SPRE_17BITS
*/
i2_error i2_rtc_wakeup_set(uint32_t period)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;
  uint32_t clock_source;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  if (period > 0xffff) {
    if (period > 0x1ffff) {
      period = 0xffff;
    }
    clock_source = RTC_WAKEUPCLOCK_CK_SPRE_17BITS;
  } else {
    clock_source = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
  }

  HAL_RTCEx_BKUPWrite(handle, RTC_WAKEUP_CLK_SRC_REG, clock_source);

  if (HAL_RTCEx_SetWakeUpTimer_IT(handle, period,  clock_source) != HAL_OK) {
    retval = I2_FAILURE;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_wakeup_get(uint32_t *period)
{
  RTC_HandleTypeDef *handle;
  i2_error retval = I2_SUCCESS;
  uint32_t clock_source;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  clock_source = HAL_RTCEx_BKUPRead(handle, RTC_WAKEUP_CLK_SRC_REG);
  *period = HAL_RTCEx_GetWakeUpTimer(handle);
  if ( clock_source == RTC_WAKEUPCLOCK_CK_SPRE_17BITS ) {
    *period |= 0x10000;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_wakeup_delete(void)
{
  i2_error retval = I2_SUCCESS;
  RTC_HandleTypeDef *handle;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Get the pointer to the RTC handle */
  handle = &(ctx.rtc);

  if(HAL_RTCEx_DeactivateWakeUpTimer(handle) != HAL_OK) {
    retval = I2_FAILURE;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_wakeup_handler_register(void (*cb)(void *arg), void *arg)
{
  i2_error retval = I2_SUCCESS;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  ctx.wakeup.cb = cb;
  ctx.wakeup.arg = arg;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_wakeup_handler_unregister(void)
{
  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  ctx.wakeup.cb = NULL;
  ctx.wakeup.arg = NULL;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return I2_SUCCESS;
}

i2_error i2_rtc_alarm_handler_register(rtc_alarm_t alarm_id,
                                       void (*cb)(void *arg), void *arg)
{
  i2_error retval = I2_SUCCESS;

  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

  if ( (alarm_id < 0) || (alarm_id >= MAX_NUM_RTC_ALARMS) ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  /* Already registered */
  if (ctx.alarm[alarm_id].cb) {
    retval = I2_NOT_AVAILABLE;
    goto out;
  }

  ctx.alarm[alarm_id].cb = cb;
  ctx.alarm[alarm_id].arg = arg;

out:
#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return retval;
}

i2_error i2_rtc_alarm_handler_unregister(rtc_alarm_t alarm_id)
{
  if ( !ctx.initialized ) {
    return I2_INVALID_PARAM;
  }

  if ( (alarm_id < 0) || (alarm_id >= MAX_NUM_RTC_ALARMS) ) {
    return I2_INVALID_PARAM;
  }

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreTake(ctx.mutex, (TickType_t)portMAX_DELAY);
#endif

  ctx.alarm[alarm_id].cb = NULL;
  ctx.alarm[alarm_id].arg = NULL;

#if defined ( ENABLE_RTOS_AWARE_HAL )
  xSemaphoreGive(ctx.mutex);
#endif

  return I2_SUCCESS;
}

/* This handles both alarm callbacks */
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(rtc);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *handle)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif

  if ( ctx.initialized ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    if (ctx.alarm[RTC_ALARM_1].cb) {
      ctx.alarm[RTC_ALARM_1].cb(ctx.alarm[RTC_ALARM_1].arg);
    }
  }
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *handle)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif

  if ( ctx.initialized ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    if ( ctx.alarm[RTC_ALARM_2].cb ) {
      ctx.alarm[RTC_ALARM_2].cb(ctx.alarm[RTC_ALARM_2].arg);
    }
  }
}

void RTC_WKUP_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(rtc);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *handle)
{
#if defined ( ENABLE_RTOS_AWARE_HAL )
  static BaseType_t xHigherPriorityTaskWoken;
#endif

  if ( ctx.initialized ) {
#if defined ( ENABLE_RTOS_AWARE_HAL )
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    if (ctx.wakeup.cb) {
      ctx.wakeup.cb(ctx.wakeup.arg);
    }
  }
}

/************************ (C) COPYRIGHT iota2 ************END OF FILE**********/
