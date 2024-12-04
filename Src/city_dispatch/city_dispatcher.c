 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_dispatcher.h"

/* Definitions for city dispatcher task */
static const uint16_t DISPATCHER_TIMEOUT_MS = 30000;

const static osThreadAttr_t cityDispatcherTask_attributes = {
  .name = "cityDispatcherTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t cityDispatcherTaskHandle;
static osStatus_t queue_read_status;
static CityEvent_t current_event_buffer;
static char output_buffer[48];

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
    event_tracker_initialize();
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

	serial_printer_spool_string((String_t *)&msg_task_waiting);

	for(;;)
	{
        // first check the high priority inbox
		queue_read_status = osMessageQueueGet(city_inbox.inboxHighPriorityQueueHandle, &current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));

        // then medium
        if (queue_read_status != osOK)
        {
            queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));
        }

        // then low
        if (queue_read_status != osOK)
        {
            queue_read_status = osMessageQueueGet(city_inbox.inboxLowPriorityQueueHandle, &current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));
        }

		if (queue_read_status == osOK)
		{
			serial_printer_spool_string((String_t *)&msg_task_received);
			serial_printer_spool_string((String_t *)&(eventTemplates[current_event_buffer.eventTemplateIndex].description));

            // stalls dispatcher until event tracker space clears
            // TODO: implement disposing of lower priority events
            while(event_tracker_get_remaining_storage() < 1)
            {
                osDelay(pdMS_TO_TICKS(100));
                event_tracker_refresh();
            }

            CityEvent_t *trackedEvent = event_tracker_add(current_event_buffer);

            for (uint8_t idx = 0; idx < NUM_EVENT_JOBS; idx++)
            {
                if (trackedEvent->jobs[idx].status == JOB_PENDING)
                {
                    osMessageQueuePut(*(department_inboxes[trackedEvent->jobs[idx].code), &&(trackedEvent->jobs[idx]), 0, osWaitForever);
                }
            }
		}
		else
		{
			osDelay(pdMS_TO_TICKS(200));
			continue;
		}
		serial_printer_spool_string((String_t *)&msg_task_waiting);
	}
}

