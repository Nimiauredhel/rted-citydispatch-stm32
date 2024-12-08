/*
 * simulation_control.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "simulation_control.h"

static void simulation_control_task(void *argument);
static void simulation_initialize();
static void simulation_start();
static void simulation_stop();

/* Definitions for control task */
osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .stack_size = TASK_STACK_SIZE*2,
  .priority = (osPriority_t) CONTROL_PRIORITY,
};

osSemaphoreId_t userInputSemHandle;
StaticSemaphore_t userInputSemControlBlock;
const osSemaphoreAttr_t userInputSem_attributes = {
  .name = "userInputSem",
  .cb_mem = &userInputSemControlBlock,
  .cb_size = sizeof(userInputSemControlBlock),
};

/* Consts for control task */
static const char *sim_separator = "\n\r\n\r--------------------------------\n\r\n\r";
static const char *control_prompt = "\n\rInput 's' to start, 'h' to stop,\n\r't' to set date and time, 'r' to restart.\n\r";

/* Variables */
extern UART_HandleTypeDef huart3;

static HAL_StatusTypeDef rxStatus;
static char input = '~';
static bool running = false;
static CityLog_t log_buffer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		osSemaphoreRelease(userInputSemHandle);
	}
}

void simulation_start_control_task()
{
	output_print_blocking_autosize(sim_separator);
	// presetting as many log parameters as possible here
	// so that they don't have to go on the task's stack at any point
	log_buffer.identifier_0 = LOGID_CONTROL;
	log_buffer.subject_0 = LOGSBJ_SIMULATION;
	log_buffer.subject_1 = LOGSBJ_BACKSPACE;
	log_buffer.subject_2 = LOGSBJ_USER_INPUT;

	userInputSemHandle = osSemaphoreNew(1, 0, &userInputSem_attributes);
	controlTaskHandle = osThreadNew(simulation_control_task, NULL, &controlTask_attributes);
}

/* static functions */

/**
  * @brief  Function implementing the simulation control task thread.
  * @param  argument: Not used
  * @retval None
  */
static void simulation_control_task(void *argument)
{
	simulation_initialize();
	output_print_blocking_autosize(control_prompt);

	for(;;)
	{
		osDelay(DELAY_100MS_TICKS);
		rxStatus = HAL_UART_Receive_IT(&huart3, (uint8_t *)&input, 1);
		osSemaphoreAcquire(userInputSemHandle, osWaitForever);

		if (running)
		{
			if (input == 'h')
			{
				running = false;
				simulation_stop();
				osDelay(DELAY_100MS_TICKS);
				output_print_blocking_autosize(control_prompt);
			}
		}
		else
		{
			switch (input)
			{
				case 's':
					running = true;
					simulation_start();
					osDelay(DELAY_100MS_TICKS);
					break;
				case 't':
					date_time_set();
					output_print_blocking_autosize(control_prompt);
					break;
				case 'r':
					HAL_NVIC_SystemReset();
					break;
				default:
					break;
			}
		}

		input = '~';
	}
}

static void simulation_initialize()
{
    output_initialize();
	serial_printer_initialize();
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_INITIALIZING;
	serial_printer_spool_log(&log_buffer);
	osDelay(DELAY_100MS_TICKS);
	city_departments_initialize();
	osDelay(DELAY_100MS_TICKS);
	city_inbox_initialize();
    event_tracker_initialize();
	event_gen_initialize();
	city_dispatcher_initialize();
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(&log_buffer);
	osDelay(DELAY_100MS_TICKS);
}

static void simulation_start()
{
	log_buffer.format = LOGFMT_STARTING_SUBJECT;
	serial_printer_spool_log(&log_buffer);
	osDelay(DELAY_10MS_TICKS);
	city_departments_start();
	osDelay(DELAY_10MS_TICKS);
	city_dispatcher_start();
	osDelay(DELAY_10MS_TICKS);
	event_gen_start();
}

static void simulation_stop()
{
	event_gen_stop();
	city_dispatcher_stop();
	city_departments_stop();
	osDelay(DELAY_10MS_TICKS);
	log_buffer.format = LOGFMT_STOPPED_SUBJECT;
	serial_printer_spool_log(&log_buffer);
}
