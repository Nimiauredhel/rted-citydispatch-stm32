/*
 * simulation_output.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "serial_output.h"

/*
 * Public serial printer variables
 */
extern UART_HandleTypeDef huart3;

/*
 * Private serial printer variables
 */
static const uint16_t PRINTER_TIMEOUT_MS = 5000;

static const osThreadAttr_t serialPrinterTask_attributes = {
  .name = "serialPrinterTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t serialPrinterTaskHandle;

static osMessageQueueId_t serialPrinterQueueHandle;
static uint8_t serialPrinterQueueBuffer[ INCOMING_QUEUE_LENGTH * sizeof( CityEvent_t ) ];
static StaticQueue_t serialPrinterQueueControlBlock;

static const osMessageQueueAttr_t serialPrinterQueue_attributes = {
  .name = "serialPrinterQueue",
  .cb_mem = &serialPrinterQueueControlBlock,
  .cb_size = sizeof(serialPrinterQueueControlBlock),
  .mq_mem = &serialPrinterQueueBuffer,
  .mq_size = sizeof(serialPrinterQueueBuffer)
};

static osStatus_t queue_read_status;
static String_t *current_string;
static char output_buffer[48];

static const String22_t msg_task_init =
{
	.size = 23,
	.text = "Serial printer init.\n\r"
};

static void serial_printer_task();

void serial_printer_initialize()
{
  serialPrinterQueueHandle = osMessageQueueNew (OUTPUT_QUEUE_LENGTH, sizeof(String_t *), &serialPrinterQueue_attributes);
  serialPrinterTaskHandle = osThreadNew(serial_printer_task, NULL, &serialPrinterTask_attributes);
}

void serial_printer_spool_chars(char *string)
{
	String_t *new_string = utils_structure_string_alloc(string);
	osMessageQueuePut(serialPrinterQueueHandle, &new_string, 0, osWaitForever);
}
void serial_printer_spool_string(String_t *string)
{
	osMessageQueuePut(serialPrinterQueueHandle, &string, 0, osWaitForever);
}

static void output_print_blocking_autosize(char *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, strlen(string)+1, 0xFFFF);
	}
}

static void output_print_irq(char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, size);
	}
}

static void output_print_string_irq(String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string->text, string->size);
	}
}

static void output_print_irq_autosize(char *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, strlen(string)+1);
	}
}

static void output_print_blocking(char *string, uint16_t size)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string, size, 0xFFFF);
	}
}

static void output_print_string_blocking(String_t *string)
{
	HAL_StatusTypeDef transmitStatus = HAL_BUSY;
	while (transmitStatus == HAL_BUSY)
	{
		transmitStatus = HAL_UART_Transmit(&huart3, (uint8_t *)string->text, string->size, 0xFFFF);
	}
}

static void serial_printer_task()
{
	output_print_string_blocking((String_t *)&msg_task_init);
	osDelay(pdMS_TO_TICKS(500));

	for(;;)
	{
		queue_read_status = osMessageQueueGet(serialPrinterQueueHandle, &current_string, NULL, pdMS_TO_TICKS(PRINTER_TIMEOUT_MS));

		if (queue_read_status == osErrorTimeout)
		{
			sprintf(output_buffer, "Printer timed out after %hums\n\r", PRINTER_TIMEOUT_MS);
			output_print_blocking_autosize(output_buffer);
		}
		else
		{
			output_print_string_blocking((String_t *)current_string);
		}
	}
}
