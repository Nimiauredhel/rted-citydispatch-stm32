/*
 * event_gen.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "city_event_gen.h"

// the rng handle
extern RNG_HandleTypeDef hrng;

/* Definitions for event gen task */

const static osThreadAttr_t eventGenTask_attributes = {
  .name = "eventGenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t eventGenTaskHandle;
static EventGenState_t eventGenState;

static void event_gen_task();

void event_gen_initialize()
{
  eventGenTaskHandle = osThreadNew(event_gen_task, NULL, &eventGenTask_attributes);
  event_gen_stop();
}

void event_gen_start()
{
	osThreadResume(eventGenTaskHandle);
}

void event_gen_stop()
{
	osThreadSuspend(eventGenTaskHandle);
}

static void generate_event()
{
    uint16_t expirationSecs;
    // randomly select event template index
	HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
	eventGenState.next_idx =
		(eventGenState.random_number & (NUM_EVENT_TEMPLATES-1));

    // apply event template to generated event
    eventGenState.generated_event.eventTemplateIndex = eventGenState.next_idx;

    // generate random expiration time & date according to template
    // first get the current time and date
    eventGenState.generated_event.expirationTime = time_get();
    eventGenState.generated_event.expirationDate = date_get();
    // generate the expiration interval from RNG
    HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
    expirationSecs =
        (eventGenState.random_number
         & (eventTemplates[eventGenState.next_idx].maxSecsToExpire - eventTemplates[eventGenState.next_idx].minSecsToExpire))
        + eventTemplates[eventGenState.next_idx].minSecsToExpire;
    // TODO: add expiration interval to current time & date

    // apply job templates from event template
    // to jobs in generated event
    for (uint8_t idx = 0; idx < NUM_EVENT_JOBS; idx++)
    {
        // get this job's template index
        uint8_t jobTemplateIdx = eventTemplates[eventGenState.next_idx].jobTemplateIndices[idx];
        // apply the job template's department code
        eventGenState.generated_event.jobs[idx].code = jobTemplates[jobTemplateIdx].code;
        // if job department code is less than 0, it is a null job - status should be NONE
        // else, set the job status to PENDING so the dispatcher knows to queue it
        if (eventGenState.generated_event.jobs[idx].code < 0)
        {
            eventGenState.generated_event.jobs[idx].status = JOB_NONE;
        }
        else
        {
            eventGenState.generated_event.jobs[idx].status = JOB_PENDING;
            // randomly generate job handling time
            HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
            eventGenState.generated_event.jobs[idx].secsToHandle =
                (eventGenState.random_number
                 & (jobTemplates[jobTemplateIdx].maxSecsToHandle - jobTemplates[jobTemplateIdx].minSecsToHandle))
                + jobTemplates[jobTemplateIdx].minSecsToHandle;
        }
    }
}

static void event_gen_task()
{
	serial_printer_spool_chars("Event generation init.\n\r");

	for(;;)
	{
        // generate random delay in ms between min & max
		HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
        eventGenState.next_delay = 
            (eventGenState.random_number 
             & (EVENT_GENERATOR_SLEEP_MAX - EVENT_GENERATOR_SLEEP_MIN))
            + EVENT_GENERATOR_SLEEP_MIN;
        sprintf(eventGenState.output_buffer, "Next event in %lums.\n\r", eventGenState.next_delay);
        serial_printer_spool_chars(eventGenState.output_buffer);
		osDelay(eventGenState.next_delay);

		// generate event randomly from pool of templates
        generate_event();
        sprintf(eventGenState.output_buffer, "Event generated from template #%u.\n\r", eventGenState.next_idx);
        serial_printer_spool_chars(eventGenState.output_buffer);

        // place event in city dispatcher inbox
		osMessageQueuePut(city_inbox.inboxMediumPriorityQueueHandle, &eventGenState.generated_event, 0, osWaitForever);
        sprintf(eventGenState.output_buffer, "%lu events in queue.\n\r", osMessageQueueGetCount(city_inbox.inboxMediumPriorityQueueHandle));
        serial_printer_spool_chars(eventGenState.output_buffer);
	}
}
