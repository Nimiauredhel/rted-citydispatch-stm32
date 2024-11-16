/*
 * city_dispatch.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_dispatcher.h"

/* Definitions for city dispatcher task */
osThreadId_t cityDispatcherTaskHandle;
const static osThreadAttr_t cityDispatcherTask_attributes = {
  .name = "cityDispatcherTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static void city_dispatcher_task();

void city_dispatcher_initialize()
{
  cityDispatcherTaskHandle = osThreadNew(city_dispatcher_task, NULL, &cityDispatcherTask_attributes);
}

osStatus_t queue_read_status;
CityEvent_t current_event_buffer;
uint16_t timeoutMs = 5000;
char output_buffer[48];

static void city_dispatcher_task()
{
	sprintf(output_buffer, "City dispatcher init.\n\r");
	output_print_blocking(output_buffer, strlen(output_buffer) + 1);
	osDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		output_print_blocking("City dispatcher awaiting message.\n\r", 36);
		queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &current_event_buffer, NULL, pdMS_TO_TICKS(timeoutMs));

		if (queue_read_status == osErrorTimeout)
		{
			sprintf(output_buffer, "Dispatcher timed out after %hums\n\r", timeoutMs);
			output_print_blocking(output_buffer, strlen(output_buffer) + 1);
		}
		else
		{
			sprintf(output_buffer, "Message received: ");
			output_print_blocking(output_buffer, strlen(output_buffer) + 1);
			output_print_blocking(current_event_buffer.description, strlen(current_event_buffer.description) + 1);
		}
	}
}

