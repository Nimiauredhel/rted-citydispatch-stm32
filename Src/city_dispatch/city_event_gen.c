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
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
};

static osThreadId_t eventGenTaskHandle;
static uint32_t random_number;
static uint32_t next_delay;
static uint8_t next_idx;
static CityEvent_t generated_event;
static CityLog_t log_buffer;
static uint8_t jobTemplateIdx;
static uint16_t expirationSecs;

static void event_gen_task();

void event_gen_initialize()
{
	eventGenTaskHandle = osThreadNew(event_gen_task, NULL, &eventGenTask_attributes);
	event_gen_stop();
	log_buffer.identifier_0 = LOGID_EVENT_GEN;
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(&log_buffer);
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
    // randomly select event template index
	HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
	next_idx = (random_number % NUM_EVENT_TEMPLATES);

	log_buffer.format = LOGFMT_GENERATING_EVENT;
	log_buffer.subject_0 = LOGSBJ_EVENT;
	log_buffer.subject_1 = next_idx;
	serial_printer_spool_log(&log_buffer);

    // apply event template to generated event
    generated_event.eventTemplateIndex = next_idx;

    // generate random expiration time & date according to template
    // first get the current time and date
    generated_event.expirationTime = time_get();
    generated_event.expirationDate = date_get();
    // generate the expiration interval from RNG
    HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
    expirationSecs =
        (random_number
         & (eventTemplates[next_idx].maxSecsToExpire - eventTemplates[next_idx].minSecsToExpire))
        + eventTemplates[next_idx].minSecsToExpire;
    // TODO: add expiration interval to current time & date

    // apply job templates from event template
    // to jobs in generated event
    for (uint8_t idx = 0; idx < NUM_EVENT_JOBS; idx++)
    {
        // get this job's template index
        jobTemplateIdx = eventTemplates[next_idx].jobTemplateIndices[idx];
        // apply the job template's department code
        generated_event.jobs[idx].code = jobTemplates[jobTemplateIdx].code;
		generated_event.jobs[idx].jobTemplateIndex = jobTemplateIdx;
        // if job department code is less than 0, it is a null job - status should be NONE
        // else, set the job status to PENDING so the dispatcher knows to queue it
        if (generated_event.jobs[idx].code < 0)
        {
            generated_event.jobs[idx].status = JOB_NONE;
        }
        else
        {
            generated_event.jobs[idx].status = JOB_PENDING;
            // randomly generate job handling time
            HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
            generated_event.jobs[idx].secsToHandle =
                (random_number
                 & (jobTemplates[jobTemplateIdx].maxSecsToHandle - jobTemplates[jobTemplateIdx].minSecsToHandle))
                + jobTemplates[jobTemplateIdx].minSecsToHandle;
        }
    }
}

static void event_gen_task()
{
	for(;;)
	{
        // generate random delay in ms between min & max
		HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
        next_delay =
            (random_number
             & (EVENT_GENERATOR_SLEEP_MAX - EVENT_GENERATOR_SLEEP_MIN))
            + EVENT_GENERATOR_SLEEP_MIN;

        //sprintf(output_buffer, "Next event in %lums.\n\r", next_delay);
        //serial_printer_spool_chars(output_buffer);

		osDelay(pdMS_TO_TICKS(next_delay));

		// generate event randomly from pool of templates
        generate_event();

        // place event in city dispatcher inbox
		osMessageQueuePut(city_inbox.inboxMediumPriorityQueueHandle, &generated_event, 0, osWaitForever);

        //sprintf(output_buffer, "%lu events in queue.\n\r", osMessageQueueGetCount(city_inbox.inboxMediumPriorityQueueHandle));
        //serial_printer_spool_chars(output_buffer);
	}
}
