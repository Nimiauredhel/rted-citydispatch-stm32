/*
 * simulation_output.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "serial_output.h"

extern UART_HandleTypeDef huart3;

void output_print_blocking(char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, size, 0xFFFF);
	}
}

void output_print_string_blocking(String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string->text, string->size, 0xFFFF);
	}
}

void output_print_blocking_autosize(char *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, strlen(string)+1, 0xFFFF);
	}
}

void output_print_irq(char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, size);
	}
}

void output_print_string_irq(String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string->text, string->size);
	}
}

void output_print_irq_autosize(char *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, strlen(string)+1);
	}
}
