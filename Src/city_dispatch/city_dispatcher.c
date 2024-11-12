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
size_t desc_length = 0;

static void city_dispatcher_task()
{
	output_print_blocking("City dispatcher init.\n\r", 24);
	osDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		output_print_blocking("City dispatcher awaiting message.\n\r", 36);
		queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &current_event_buffer, NULL, osWaitForever);
		output_print_blocking("Message received!\n\r", 20);
		desc_length = strlen(current_event_buffer.description) + 1;
		output_print_blocking(current_event_buffer.description, desc_length);
	}
}

