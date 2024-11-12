/*
 * event_gen.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "event_gen.h"

/* Definitions for event gen task */
osThreadId_t eventGenTaskHandle;
const static osThreadAttr_t eventGenTask_attributes = {
  .name = "eventGenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static void event_gen_task();

CityEvent_t test_event;
char *test_event_message = "This is a test event.\n\r";


void event_gen_initialize()
{
  test_event.code = 1;
  test_event.description = test_event_message;
  test_event.ticks = pdMS_TO_TICKS(1000);
  eventGenTaskHandle = osThreadNew(event_gen_task, NULL, &eventGenTask_attributes);
}


static void event_gen_task()
{
	output_print_blocking("Event generation init.\n\r", 24);

	for(;;)
	{
		osDelay(pdMS_TO_TICKS(2000));
		output_print_blocking("Event generation.\n\r", 19);
		osMessageQueuePut(city_inbox.inboxMediumPriorityQueueHandle, &test_event, 0, pdMS_TO_TICKS(250));
	}
}
