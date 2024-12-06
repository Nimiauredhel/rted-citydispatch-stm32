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
  .priority = (osPriority_t) osPriorityNormal,
};

/* Consts for control task */
uint8_t brief_delay_ticks = pdMS_TO_TICKS(10);
static const char *sim_separator = "\n\r\n\r--------------------------------\n\r\n\r";
static const char *control_prompt = "Input 's' to start, 'h' to stop, 't' to set date and time, 'r' to restart.\n\r";

/* Variables */
extern UART_HandleTypeDef huart3;

static char input[1];
static bool running = false;
static CityLog_t log_buffer;

void simulation_start_control_task()
{
	output_print_blocking_autosize(sim_separator);
	// presetting as many log parameters as possible here
	// so that they don't have to go on the task's stack at any point
	log_buffer.identifier_0 = LOGID_CONTROL;
	log_buffer.subject_0 = LOGSBJ_SIMULATION;
	log_buffer.subject_1 = LOGSBJ_NULL;
	log_buffer.subject_2 = LOGSBJ_USER_INPUT;
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
		osDelay(brief_delay_ticks);

		if (HAL_OK == HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, pdMS_TO_TICKS(10)))
		{
			if (running
				&& input[0] == 'h')
			{
				simulation_stop();
				osDelay(brief_delay_ticks);
				running = false;
			}
			else if (!running
				&& input[0] == 's')
			{
				simulation_start();
				osDelay(brief_delay_ticks);
				running = true;
			}
			else if (input[0] == 't')
			{
				date_time_set();
			}
			else if (input[0] == 'r')
			{
				HAL_NVIC_SystemReset();
			}
		}
		else
		{
				osDelay(brief_delay_ticks);
		}
	}
}

static void simulation_initialize()
{
	serial_printer_initialize();
	osDelay(brief_delay_ticks);
	log_buffer.format = LOGFMT_INITIALIZING;
	serial_printer_spool_log(&log_buffer);
	osDelay(brief_delay_ticks);
	event_gen_initialize();
	osDelay(brief_delay_ticks);
	city_inbox_initialize();
	osDelay(brief_delay_ticks);
	city_dispatcher_initialize();
	osDelay(brief_delay_ticks);
	city_departments_initialize();
	osDelay(brief_delay_ticks);
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(&log_buffer);
	osDelay(brief_delay_ticks);
}

static void simulation_start()
{
	output_print_blocking_autosize(sim_separator);
	log_buffer.format = LOGFMT_STARTING_SUBJECT;
	serial_printer_spool_log(&log_buffer);
	osDelay(brief_delay_ticks);
	city_inbox_clear();
	osDelay(brief_delay_ticks);
	city_dispatcher_start();
	osDelay(brief_delay_ticks);
	city_departments_start();
	osDelay(brief_delay_ticks);
	event_gen_start();
	osDelay(brief_delay_ticks);
	log_buffer.format = LOGFMT_STARTED_SUBJECT;
	serial_printer_spool_log(&log_buffer);
}

static void simulation_stop()
{
	log_buffer.format = LOGFMT_STOPPING_SUBJECT;
	serial_printer_spool_log(&log_buffer);
	osDelay(brief_delay_ticks);
	event_gen_stop();
	osDelay(brief_delay_ticks);
	city_dispatcher_stop();
	osDelay(brief_delay_ticks);
	city_departments_stop();
	osDelay(brief_delay_ticks);
	city_inbox_clear();
	osDelay(brief_delay_ticks);
	log_buffer.format = LOGFMT_STOPPED_SUBJECT;
	serial_printer_spool_log(&log_buffer);
	output_print_blocking_autosize(sim_separator);
}
