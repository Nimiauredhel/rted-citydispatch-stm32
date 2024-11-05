/*
 * simulation_control.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "simulation_control.h"

static void simulation_control_task(void *argument);
static void simulation_start();
static void simulation_stop();

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
  output_print_blocking("Control task.\n\r", 15);
  osDelay(pdMS_TO_TICKS(1000));
  simulation_start();

  for(;;)
  {
    osDelay(pdMS_TO_TICKS(1000));
  }
}

static void simulation_start()
{
  output_print_blocking("Simulation start.\n\r", 19);
  event_gen_create_task();
}

static void simulation_stop()
{
  output_print_blocking("Simulation stop.\n\r", 18);
}
