/*
 * serial_output.c
 *
 *  Created on: Nov 21, 2024
 *      Author: mickey
 */

#include "serial_output.h"

extern UART_HandleTypeDef huart3;

osMutexId_t serialOutputMutexHandle;
StaticSemaphore_t serialOutputMutexControlBlock;
const osMutexAttr_t serialOutputMutex_attributes = {
  .name = "serialOutputMutex",
  .cb_mem = &serialOutputMutexControlBlock,
  .cb_size = sizeof(serialOutputMutexControlBlock),
};

static const TickType_t serial_timeout = pdMS_TO_TICKS(1000);

void output_initialize()
{
    serialOutputMutexHandle = osMutexNew(&serialOutputMutex_attributes);
}

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

void output_print_irq_autosize(const char *string)
{
	uint16_t size = strlen(string)+1;
	output_print_irq(string, size);
}

void output_print_blocking(const char *string, uint16_t size)
{
    osMutexAcquire(serialOutputMutexHandle, osWaitForever);

	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, size, osWaitForever);
	}

    osMutexRelease(serialOutputMutexHandle);
}
