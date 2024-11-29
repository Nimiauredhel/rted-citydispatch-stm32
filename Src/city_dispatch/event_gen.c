/*
 * event_gen.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "event_gen.h"

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
	HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
	eventGenState.next_idx =
		(eventGenState.random_number & (NUM_EVENT_TEMPLATES-1));
	eventGenState.generated_event.code = eventTemplates[eventGenState.next_idx].code;
	eventGenState.generated_event.description = utils_structure_string46(eventTemplates[eventGenState.next_idx].description);

	HAL_RNG_GenerateRandomNumber(&hrng, &eventGenState.random_number);
        eventGenState.generated_event.ticks =
            (eventGenState.random_number
             & (eventTemplates[eventGenState.next_idx].maxTicks - eventTemplates[eventGenState.next_idx].minTicks))
            + eventTemplates[eventGenState.next_idx].minTicks;
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
