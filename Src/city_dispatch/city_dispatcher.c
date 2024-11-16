/*
 * city_dispatch.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_dispatcher.h"

/* Definitions for city dispatcher task */
static const uint16_t DISPATCHER_TIMEOUT_MS = 5000;

const static osThreadAttr_t cityDispatcherTask_attributes = {
  .name = "cityDispatcherTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t cityDispatcherTaskHandle;
static DispatcherState_t dispatcherState;

static void city_dispatcher_task();

void city_dispatcher_initialize()
{
  cityDispatcherTaskHandle = osThreadNew(city_dispatcher_task, NULL, &cityDispatcherTask_attributes);
}

static void city_dispatcher_task()
{
	sprintf(dispatcherState.output_buffer, "City dispatcher init.\n\r");
	output_print_blocking_autosize(dispatcherState.output_buffer);
	osDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		output_print_blocking_autosize("City dispatcher awaiting message.\n\r");
		dispatcherState.queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &dispatcherState.current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));

		if (dispatcherState.queue_read_status == osErrorTimeout)
		{
			sprintf(dispatcherState.output_buffer, "Dispatcher timed out after %hums\n\r", DISPATCHER_TIMEOUT_MS);
			output_print_blocking_autosize(dispatcherState.output_buffer);
		}
		else
		{
			sprintf(dispatcherState.output_buffer, "Message received!\r\n");
			output_print_blocking_autosize(dispatcherState.output_buffer);
			output_print_blocking_autosize(dispatcherState.current_event_buffer.description);
		}
	}
}

