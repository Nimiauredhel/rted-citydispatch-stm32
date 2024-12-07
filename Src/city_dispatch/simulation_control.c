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

/* Consts for control task */
static const char *sim_separator = "\n\r\n\r--------------------------------\n\r\n\r";
static const char *control_prompt = "Input 's' to start, 'h' to stop, 't' to set date and time, 'r' to restart.\n\r";

/* Variables */
extern UART_HandleTypeDef huart3;

static char input = '~';
static bool running = false;
static CityLog_t log_buffer;

void simulation_start_control_task()
{
	output_print_blocking_autosize(sim_separator);
	// presetting as many log parameters as possible here
	// so that they don't have to go on the task's stack at any point
	log_buffer.identifier_0 = LOGID_CONTROL;
	log_buffer.subject_0 = LOGSBJ_SIMULATION;
	log_buffer.subject_1 = LOGSBJ_BACKSPACE;
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
		osDelay(DELAY_300MS_TICKS);

		if (HAL_OK == HAL_UART_Receive(&huart3, (uint8_t *)&input, 1, DELAY_10MS_TICKS))
		{
			if (running
				&& input == 'h')
			{
				running = false;
				simulation_stop();
				osDelay(DELAY_100MS_TICKS);
			}
			else if (!running
				&& input == 's')
			{
				running = true;
				simulation_start();
				osDelay(DELAY_100MS_TICKS);
			}
			else if (input == 't')
			{
				date_time_set();
			}
			else if (input == 'r')
			{
				HAL_NVIC_SystemReset();
			}

			osDelay(DELAY_300MS_TICKS);
            input = '~';
		}
	}
}

static void simulation_initialize()
{
	serial_printer_initialize();
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_INITIALIZING;
	serial_printer_spool_log(&log_buffer);
	osDelay(DELAY_100MS_TICKS);
	city_departments_initialize();
	osDelay(DELAY_100MS_TICKS);
	city_inbox_initialize();
	osDelay(DELAY_100MS_TICKS);
    event_tracker_initialize();
	osDelay(DELAY_100MS_TICKS);
	event_gen_initialize();
	osDelay(DELAY_100MS_TICKS);
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
	city_inbox_clear();
	osDelay(DELAY_10MS_TICKS);
	city_dispatcher_start();
	osDelay(DELAY_10MS_TICKS);
	city_departments_start();
	osDelay(DELAY_10MS_TICKS);
	event_gen_start();
	osDelay(DELAY_10MS_TICKS);
	log_buffer.format = LOGFMT_STARTED_SUBJECT;
	serial_printer_spool_log(&log_buffer);
}

static void simulation_stop()
{
	log_buffer.format = LOGFMT_STOPPING_SUBJECT;
	serial_printer_spool_log(&log_buffer);
	osDelay(DELAY_10MS_TICKS);
	event_gen_stop();
	osDelay(DELAY_10MS_TICKS);
	city_dispatcher_stop();
	osDelay(DELAY_10MS_TICKS);
	city_departments_stop();
	osDelay(DELAY_10MS_TICKS);
	city_inbox_clear();
	osDelay(DELAY_10MS_TICKS);
	log_buffer.format = LOGFMT_STOPPED_SUBJECT;
	serial_printer_spool_log(&log_buffer);
}
