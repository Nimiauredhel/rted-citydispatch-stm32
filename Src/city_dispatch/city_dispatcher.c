/*
*
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_dispatcher.h"

/* Definitions for city dispatcher task */

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
	osDelay(DELAY_100MS_TICKS);
	log_buffer.format = LOGFMT_TASK_STARTING;
	serial_printer_spool_log(log_buffer);

	//osDelay(pdMS_TO_TICKS(100));
	//log_buffer.format = LOGFMT_WAITING;
	//serial_printer_spool_log(&log_buffer);

	for(;;)
	{
		// periodically refresh the event tracker if "dirty"
		if (event_tracker_get_dirty())
		{
			event_tracker_refresh();
		}

        // first check the high priority inbox
		queue_read_status = osMessageQueueGet(city_inbox.inboxHighPriorityQueueHandle, &current_event_buffer, NULL, DELAY_100MS_TICKS);

        // then medium
        if (queue_read_status != osOK)
        {
            queue_read_status = osMessageQueueGet(city_inbox.inboxMediumPriorityQueueHandle, &current_event_buffer, NULL, DELAY_10MS_TICKS);
        }

        // then low
        if (queue_read_status != osOK)
        {
            queue_read_status = osMessageQueueGet(city_inbox.inboxLowPriorityQueueHandle, &current_event_buffer, NULL, DELAY_10MS_TICKS);
        }

		if (queue_read_status == osOK)
		{
			log_buffer.format = LOGFMT_RECEIVED;
			log_buffer.subject_1 = current_event_buffer.eventTemplateIndex;

            // stalls dispatcher until event tracker space clears
            // TODO: implement disposing of lower priority events
            if(event_tracker_get_remaining_storage() < 1)
            {
                osDelay(DELAY_100MS_TICKS);
                event_tracker_refresh();
            }

            trackedEvent = event_tracker_add(current_event_buffer);

            if (trackedEvent == NULL)
            {
            	// TODO: log about dropped event
				osDelay(DELAY_10MS_TICKS);
				continue;
            }

            for (uint8_t idx = 0; idx < NUM_EVENT_JOBS; idx++)
            {
				trackedJob = &(trackedEvent->jobs[idx]);
				if (trackedJob->code == DEPT_EMPTY
					|| trackedJob->status == JOB_NONE) continue;

				DepartmentCode_t targetDepartment = trackedJob->code;

				if (department_loads[targetDepartment] == DEPARTMENT_LOAD_MAX)
				{
					for (uint8_t deps = 0; deps < NUM_DEPARTMENTS; deps++)
					{
						if (department_loads[deps] == DEPARTMENT_LOAD_FREE)
						{
							targetDepartment = deps;

							log_buffer.format = LOGFMT_BORROWING_RESOURCE;
							log_buffer.identifier_0 = LOGID_DEPARTMENT;
							log_buffer.identifier_1 = trackedJob->code;
							log_buffer.subject_0 = LOGID_DEPARTMENT;
							log_buffer.subject_1 = targetDepartment;
							serial_printer_spool_log(log_buffer);
							log_buffer.identifier_0 = LOGID_DISPATCHER;
							log_buffer.subject_0 = LOGSBJ_EVENT;

							break;
						}
					}
				}

				osMessageQueuePut(*(department_inboxes[targetDepartment]), &trackedJob, 0, osWaitForever);
            }
		}
		else
		{
			// TODO: maybe more sophisticated timeout behavior
			osDelay(DELAY_10MS_TICKS);
			continue;
		}

		//log_buffer.format = LOGFMT_WAITING;
		//serial_printer_spool_log(&log_buffer);
	}
}

