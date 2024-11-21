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
  osDelay(pdMS_TO_TICKS(1000));
  simulation_start();

  for(;;)
  {
    osDelay(pdMS_TO_TICKS(1000));
  }
}

String22_t msg_sim_start = {.size = 19, .text = "Simulation start.\n\r"};

static void simulation_start()
{
  serial_printer_initialize();
  serial_printer_spool_string((String_t *)&msg_sim_start);
  city_inbox_initialize();
  city_dispatcher_initialize();
  event_gen_initialize();
}

static void simulation_stop()
{
  serial_printer_spool_chars("Simulation stop.\n\r");
}
