/*
 * date_time.c
 *
 *  Created on: Nov 22, 2024
 *      Author: mickey
 */

#include "date_time.h"

extern UART_HandleTypeDef huart3;
extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef time_now;
RTC_DateTypeDef date_now;

// TODO: fix all the damn printfs here

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
	printf("\n\rThe current date is: %02u/%02u/%02u.\n\r", date_now.Date, date_now.Month, date_now.Year);
	printf("The current time is: %02u:%02u:%02u.\n\r", time_now.Hours, time_now.Minutes, time_now.Seconds);
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

	printf("Let's set the date.\n\rAll values are two digits including leading zero.\n\r");
	printf("enter year (yes two digits, STM is not Y2K prepared): ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	printf("%c", input[0]);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	printf("%c", input[1]);

	num = atoi(input);
	sDate.Year = num;
	osDelay(100);

	num = 30;
	while (num < 1 || num > 12)
	{
		printf("\n\renter month: ");
		HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
		printf("%c", input[0]);
		osDelay(100);
		HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
		printf("%c", input[1]);

		num = atoi(input);
	}

	sDate.Month = num;
	osDelay(100);

	printf("\n\renter day of the month: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	printf("%c", input[0]);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	printf("%c", input[1]);

	num = atoi(input);
	sDate.Date = num;
	osDelay(100);

	printf("Let's set the time.\n\rAll values are two digits including leading zero.\n\r");
	printf("enter hour: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	printf("%c", input[0]);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	printf("%c", input[1]);

	num = atoi(input);
	sTime.Hours = num;
	osDelay(100);

	printf("\n\renter minutes: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	printf("%c", input[0]);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	printf("%c", input[1]);

	num = atoi(input);
	sTime.Minutes = num;
	osDelay(100);

	printf("\n\renter seconds: ");
	HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, 0xFFFF);
	printf("%c", input[0]);
	osDelay(100);
	HAL_UART_Receive(&huart3, (uint8_t *)&input[1], 1, 0xFFFF);
	printf("%c", input[1]);

	num = atoi(input);
	sTime.Seconds = num;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("Error setting the date, apparently.\n\r");
		//Error_Handler();
	}

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("Error setting the time, apparently.\n\r");
		//Error_Handler();
	}

	date_time_alarm_reset();
	printf("\n\rLet's wait a second...\n\r");
	osDelay(pdMS_TO_TICKS(1100));
	date_time_print();
	osDelay(100);
}
