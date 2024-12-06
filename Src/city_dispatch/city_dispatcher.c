/*
*
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_dispatcher.h"

/* Definitions for city dispatcher task */
static const uint16_t DISPATCHER_TIMEOUT_MS = 300;

const static osThreadAttr_t cityDispatcherTask_attributes = {
  .name = "cityDispatcherTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) CITY_DISPATCHER_PRIORITY,
};

static CityLog_t log_buffer;
static osThreadId_t cityDispatcherTaskHandle;
static osStatus_t queue_read_status;
static CityEvent_t current_event_buffer;
static CityEvent_t *trackedEvent;
static CityJob_t *trackedJob;

static void city_dispatcher_task();

void city_dispatcher_initialize()
{
	log_buffer.identifier_0 = LOGID_DISPATCHER;
	log_buffer.subject_0 = LOGSBJ_EVENT;

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
	osDelay(pdMS_TO_TICKS(100));
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(&log_buffer);

	//osDelay(pdMS_TO_TICKS(100));
	//log_buffer.format = LOGFMT_WAITING;
	//serial_printer_spool_log(&log_buffer);

	for(;;)
	{
		// periodically refresh the event tracker if not empty
		if (event_tracker_get_length() > 0)
		{
			event_tracker_refresh();
		}

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
			log_buffer.format = LOGFMT_RECEIVED;
			log_buffer.subject_1 = current_event_buffer.eventTemplateIndex;

            // stalls dispatcher until event tracker space clears
            // TODO: implement disposing of lower priority events
            if(event_tracker_get_remaining_storage() < 1)
            {
                osDelay(pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));
                event_tracker_refresh();
            }

            trackedEvent = event_tracker_add(current_event_buffer);

            if (trackedEvent == NULL)
            {
            	// TODO: log about dropped event
				osDelay(pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));
				continue;
            }

            for (uint8_t idx = 0; idx < NUM_EVENT_JOBS; idx++)
            {
				trackedJob = &(trackedEvent->jobs[idx]);
				if (trackedJob->code == DEPT_EMPTY
					|| trackedJob->status == JOB_NONE) continue;

				osMessageQueuePut(*(department_inboxes[trackedJob->code]), &trackedJob, 0, osWaitForever);
            }
		}
		else
		{
			osDelay(pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));
			continue;
		}

		//log_buffer.format = LOGFMT_WAITING;
		//serial_printer_spool_log(&log_buffer);
	}
}

