/*
 * simulation_control.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

/** @addtogroup City_Dispatch
  * @{
  */

  ******************************************************************************
  /**
  * @file : simulation_control.c
  * @brief The "Simulation Control" task and associated USART callback.
  * which together initialize the simulation, then receive and process user input
  * to start and stop at it, and a few other utilities.
  ******************************************************************************
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
static const char *control_prompt = "\n\rInput 's' to start, 'h' to stop,\n\r't' to set date and time, 'r' to restart.\n\r";

/* Variables */

// usart3 handle for reading serial input
extern UART_HandleTypeDef huart3;

static HAL_StatusTypeDef rxStatus;
static char input = '~';
static bool running = false;
static CityLog_t log_buffer;

/**
  * @brief callback for catching user input via USART
  *        and allowing the control task to handle it.
  * @param huart: the UART handle associated with the callback.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		if (osThreadGetState(controlTaskHandle) == osThreadBlocked)
		{
			xTaskResumeFromISR(controlTaskHandle);
		}
	}
}

/**
  * @brief the kickstart function for the entire simulation,
  *        creating the simulation control task.
  * @retval None
  */
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
  * This task first initializes (and pauses) the other simulation tasks,
  * then awaits user input for starting and stopping the simulation.
  * It also allows the user to set the time & date, as well as restart the device.
  * @param  argument: Not used
  * @retval None
  */
static void simulation_control_task(void *argument)
{
	simulation_initialize();
	output_print_blocking_autosize(control_prompt);

	for(;;)
	{
        // request serial input, then suspend this task.
        // the task will be resumed via ISR once input is received.
		rxStatus = HAL_UART_Receive_IT(&huart3, (uint8_t *)&input, 1);
		osThreadSuspend(NULL);

        // once the task has been resumed,
        // handle the the now stored user input.
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
		osDelay(DELAY_100MS_TICKS);
	}
}

/**
  * @brief  Function initializing every simulation task for the first time,
  * and immediately suspending it.
  * @retval None
  */
static void simulation_initialize()
{
    output_initialize();
	serial_printer_initialize();
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_INITIALIZING;
	serial_printer_spool_log(log_buffer);
	osDelay(DELAY_100MS_TICKS);
	city_departments_initialize();
	osDelay(DELAY_100MS_TICKS);
	city_inbox_initialize();
    event_tracker_initialize();
	event_gen_initialize();
	city_dispatcher_initialize();
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(log_buffer);
	osDelay(DELAY_100MS_TICKS);
}

/**
  * @brief  Function starting (actually resuming) every simulation task.
  * @retval None
  */
static void simulation_start()
{
	log_buffer.format = LOGFMT_STARTING_SUBJECT;
	serial_printer_spool_log(log_buffer);
	osDelay(DELAY_10MS_TICKS);
	city_departments_start();
	osDelay(DELAY_10MS_TICKS);
	city_dispatcher_start();
	osDelay(DELAY_10MS_TICKS);
	event_gen_start();
}

/**
  * @brief  Function suspending every simulation task.
  * @retval None
  */
static void simulation_stop()
{
	event_gen_stop();
	city_dispatcher_stop();
	city_departments_stop();
	osDelay(DELAY_10MS_TICKS);
	log_buffer.format = LOGFMT_STOPPED_SUBJECT;
	serial_printer_spool_log(log_buffer);
}

/** 
  * @}
  */
