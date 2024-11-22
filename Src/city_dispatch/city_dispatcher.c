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

static const String22_t msg_task_init =
{
	.size = 23,
	.text = "City dispatcher init.\n\r"
};
static const String38_t msg_task_waiting =
{
	.size = 36,
	.text = "City dispatcher awaiting message.\n\r"
};
static const String22_t msg_task_received =
{
	.size = 19,
	.text = "Message received: "
};

static void city_dispatcher_task();

void city_dispatcher_initialize()
{
  cityDispatcherTaskHandle = osThreadNew(city_dispatcher_task, NULL, &cityDispatcherTask_attributes);
  city_dispatcher_stop();
}

void city_dispatcher_start()
{
	osThreadResume(cityDispatcherTaskHandle);
}

void city_dispatcher_stop()
{
  osThreadSuspend(cityDispatcherTaskHandle);
}

static void city_dispatcher_task()
{
	serial_printer_spool_string((String_t *)&msg_task_init);
	osDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		serial_printer_spool_string((String_t *)&msg_task_waiting);
		dispatcherState.queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &dispatcherState.current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));

		if (dispatcherState.queue_read_status == osErrorTimeout)
		{
			sprintf(dispatcherState.output_buffer, "Dispatcher timed out after %hums\n\r", DISPATCHER_TIMEOUT_MS);
			serial_printer_spool_chars(dispatcherState.output_buffer);
		}
		else
		{
			serial_printer_spool_string((String_t *)&msg_task_received);
			serial_printer_spool_string((String_t *)&(dispatcherState.current_event_buffer.description));
		}
	}
}
