/*
 * serial_output.c
 *
 *  Created on: Nov 21, 2024
 *      Author: mickey
 */

#include "serial_output.h"

/*
 * Public serial printer variables
 */
extern UART_HandleTypeDef huart3;

void output_print_blocking_autosize(const char *string)
{
	uint16_t size = strlen(string)+1;
	output_print_blocking(string, size);
}

void output_print_irq(const char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, size);
	}
}

void output_print_string_irq(const String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string->text, string->size);
	}
}

void output_print_irq_autosize(const char *string)
{
	uint16_t size = strlen(string)+1;
	output_print_irq(string, size);
}

void output_print_blocking(const char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, size, 0xFFFF);
	}
}

void output_print_string_blocking(const String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string->text, string->size, 0xFFFF);
	}
}

/* Retargets the C library printf function to the USART. */
#include <stdio.h>
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
#ifdef __GNUC__
int _write(int file,char *ptr, int len)
{
    int DataIdx;
    for (DataIdx= 0; DataIdx< len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}
#endif
