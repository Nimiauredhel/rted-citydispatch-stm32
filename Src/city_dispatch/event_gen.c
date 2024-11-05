/*
 * event_gen.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "event_gen.h"

/* Definitions for event gen task */
osThreadId_t eventGenTaskHandle;
const osThreadAttr_t eventGenTask_attributes = {
  .name = "eventGenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static void event_gen_task();

void event_gen_create_task()
{
  eventGenTaskHandle = osThreadNew(event_gen_task, NULL, &eventGenTask_attributes);
}

static void event_gen_task()
{
	output_print_blocking("Event generation init.\n\r", 24);

	for(;;)
	{
		osDelay(pdMS_TO_TICKS(1000));
		output_print_blocking("Event generation.\n\r", 19);
	}
}
