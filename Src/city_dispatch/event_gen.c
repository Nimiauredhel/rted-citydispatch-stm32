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
  eventGenState.test_event.code = 1;
  eventGenState.test_event.description = utils_structure_string46("This is a test event.\n\r");
  eventGenState.test_event.ticks = pdMS_TO_TICKS(1000);
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
        sprintf(eventGenState.output_buffer, "Next event in %lums\n\r", eventGenState.next_delay);
        serial_printer_spool_chars(eventGenState.output_buffer);
		osDelay(eventGenState.next_delay);
		serial_printer_spool_chars("Event generated, ");
		osMessageQueuePut(city_inbox.inboxMediumPriorityQueueHandle, &eventGenState.test_event, 0, osWaitForever);
        sprintf(eventGenState.output_buffer, "%lu events in queue.\n\r", osMessageQueueGetCount(city_inbox.inboxMediumPriorityQueueHandle));
        serial_printer_spool_chars(eventGenState.output_buffer);
	}
}
