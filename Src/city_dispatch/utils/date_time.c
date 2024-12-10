/*
 * date_time.c
 *
 *  Created on: Nov 22, 2024
 *      Author: mickey
 */

/** @addtogroup Sim_Utils
  * @{
  */

#include "date_time.h"

extern UART_HandleTypeDef huart3;
extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef time_now;
RTC_DateTypeDef date_now;
char output_buffer[64];

// TODO: fix all the damn printfs here

RTC_TimeTypeDef time_get()
{
    return time_now;
}

RTC_DateTypeDef date_get()
{
    return date_now;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	static uint8_t index = 0;
	static RTC_AlarmTypeDef sAlarm;

	// update the global timestamp every alarm
	HAL_RTC_GetTime(hrtc, &time_now, FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &date_now, FORMAT_BIN);

	// increment the alarm so it keeps going
	HAL_RTC_GetAlarm(hrtc,&sAlarm,RTC_ALARM_A,FORMAT_BIN);

	if(sAlarm.AlarmTime.Seconds < 59)
	{
		sAlarm.AlarmTime.Seconds += 1;
	}
	else
	{
		sAlarm.AlarmTime.Seconds = 0;
	}

	while(HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BIN)!=HAL_OK)
	{
	}

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, index == 0);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, index == 1 || index == 3);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, index == 2);

	index++;
	if (index > 3) index = 0;
}

void date_time_alarm_reset()
{
	static RTC_AlarmTypeDef sAlarm;

	// update the global timestamp
	HAL_RTC_GetTime(&hrtc, &time_now, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date_now, FORMAT_BIN);

	// set the alarm so it keeps going
	HAL_RTC_GetAlarm(&hrtc,&sAlarm,RTC_ALARM_A,FORMAT_BIN);

	if (time_now.Seconds >= 59) sAlarm.AlarmTime.Seconds = 0;
	else sAlarm.AlarmTime.Seconds = time_now.Seconds + 1;

	while(HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, FORMAT_BIN)!=HAL_OK)
	{
	}
}

void date_time_print()
{
	// TODO: fix this, make it useful
	sprintf(output_buffer, "\n\rThe current date is: %02u/%02u/%02u.\n\r", date_now.Date, date_now.Month, date_now.Year);
    output_print_blocking_autosize(output_buffer);
	sprintf(output_buffer, "The current time is: %02u:%02u:%02u.\n\r", time_now.Hours, time_now.Minutes, time_now.Seconds);
    output_print_blocking_autosize(output_buffer);
}

void date_time_set()
{
	RTC_DateTypeDef sDate = {0};
	RTC_TimeTypeDef sTime = {0};
	char input[2];
	uint8_t num = 0;

	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	output_print_blocking_autosize("Let's set the date.\n\rAll values are two digits including leading zero.\n\r");
	output_print_blocking_autosize("enter year (yes two digits, STM is not Y2K prepared): ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[0]);
    output_print_blocking_autosize(output_buffer);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[1]);
    output_print_blocking_autosize(output_buffer);

	num = atoi(input);
	sDate.Year = num;
	osDelay(100);

	num = 30;
	while (num < 1 || num > 12)
	{
		output_print_blocking_autosize("\n\renter month: ");
		HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
		sprintf(output_buffer, "%c", input[0]);
        output_print_blocking_autosize(output_buffer);
		osDelay(100);
		HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
		sprintf(output_buffer, "%c", input[1]);
        output_print_blocking_autosize(output_buffer);

		num = atoi(input);
	}

	sDate.Month = num;
	osDelay(100);

	output_print_blocking_autosize("\n\renter day of the month: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[0]);
	output_print_blocking_autosize(output_buffer);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[1]);
	output_print_blocking_autosize(output_buffer);

	num = atoi(input);
	sDate.Date = num;
	osDelay(100);

	output_print_blocking_autosize("\n\rLet's set the time.\n\rAll values are two digits including leading zero.\n\r");
	output_print_blocking_autosize("enter hour: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[0]);
    output_print_blocking_autosize(output_buffer);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[1]);
    output_print_blocking_autosize(output_buffer);

	num = atoi(input);
	sTime.Hours = num;
	osDelay(100);

	output_print_blocking_autosize("\n\renter minutes: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[0]);
    output_print_blocking_autosize(output_buffer);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[1]);
    output_print_blocking_autosize(output_buffer);

	num = atoi(input);
	sTime.Minutes = num;
	osDelay(100);

	output_print_blocking_autosize("\n\renter seconds: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[0]);
    output_print_blocking_autosize(output_buffer);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	sprintf(output_buffer, "%c", input[1]);
    output_print_blocking_autosize(output_buffer);

	num = atoi(input);
	sTime.Seconds = num;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		output_print_blocking_autosize("Error setting the date, apparently.\n\r");
		//Error_Handler();
	}

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		output_print_blocking_autosize("Error setting the time, apparently.\n\r");
		//Error_Handler();
	}

	date_time_alarm_reset();
	output_print_blocking_autosize("\n\rLet's wait a second...\n\r");
	osDelay(pdMS_TO_TICKS(1000));
	date_time_print();
	osDelay(pdMS_TO_TICKS(100));
}

/**
  * @}
  */
