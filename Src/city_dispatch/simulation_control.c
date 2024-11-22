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

extern UART_HandleTypeDef huart3;
static char input[1];
bool running = false;

/* Definitions for control task */
osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

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
	serial_printer_spool_chars("Input 's' to start, 'h' to stop, 't' to set date and time, 'r' to restart.\n\r");

	for(;;)
	{
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

String22_t msg_sim_start = {.size = 23, .text = "Simulation starting.\n\r"};
String22_t msg_sim_stop = {.size = 22, .text = "Simulation stopped.\n\r"};

static void simulation_initialize()
{
	serial_printer_initialize();
	city_inbox_initialize();
	city_dispatcher_initialize();
	event_gen_initialize();
	serial_printer_spool_chars("Simulation initialized.\n\r");
}

static void simulation_start()
{
	serial_printer_spool_string((String_t *)&msg_sim_start);
	city_inbox_clear();
	city_dispatcher_start();
	event_gen_start();
}

static void simulation_stop()
{
	city_dispatcher_stop();
	event_gen_stop();
	city_inbox_clear();
	serial_printer_spool_string((String_t *)&msg_sim_stop);
}
