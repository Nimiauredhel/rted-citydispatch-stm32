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

/* Variables */
extern UART_HandleTypeDef huart3;

static char input[1];
static bool running = false;

void simulation_start_control_task()
{
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
	osDelay(pdMS_TO_TICKS(1000));
	simulation_initialize();
	osDelay(pdMS_TO_TICKS(100));
	output_print_blocking_autosize("Input 's' to start, 'h' to stop, 't' to set date and time, 'r' to restart.\n\r");

	for(;;)
	{
		osDelay(pdMS_TO_TICKS(100));

		if (HAL_OK == HAL_UART_Receive(&huart3, (uint8_t *)&input[0], 1, pdMS_TO_TICKS(10)))
		{
			if (running
				&& input[0] == 'h')
			{
				running = false;
				simulation_stop();
				osDelay(pdMS_TO_TICKS(100));
			}
			else if (!running
				&& input[0] == 's')
			{
				running = true;
				simulation_start();
				osDelay(pdMS_TO_TICKS(100));
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
				osDelay(pdMS_TO_TICKS(100));
		}
	}
}

static void simulation_initialize()
{
	serial_printer_initialize();
	event_gen_initialize();
	city_inbox_initialize();
	city_dispatcher_initialize();
	city_departments_initialize();
	output_print_blocking_autosize("Simulation initialized.\n\r");
}

static void simulation_start()
{
	output_print_blocking_autosize("Simulation starting.\n\r");
	city_inbox_clear();
	city_dispatcher_start();
	city_departments_start();
	event_gen_start();
}

static void simulation_stop()
{
	event_gen_stop();
	city_dispatcher_stop();
	city_departments_stop();
	city_inbox_clear();
	output_print_blocking_autosize("Simulation stopped.\n\r");
}
