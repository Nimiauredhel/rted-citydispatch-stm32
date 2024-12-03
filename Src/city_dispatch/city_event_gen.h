/*
 * event_gen.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef CITY_EVENT_GEN_H_
#define CITY_EVENT_GEN_H_

#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "FreeRTOSConfig.h"
#include "city_inbox.h"
#include "simulation_defs.h"
#include "utils/string_utils.h"
#include "utils/date_time.h"
#include "cmsis_os.h"

#include "city_events.h"
#include "serial_printer.h"

typedef struct EventGenState
{
    uint32_t random_number;
	uint32_t next_delay;
	uint8_t next_idx;
	CityEvent_t generated_event;
    char output_buffer[32];
} EventGenState_t;

void event_gen_initialize();
void event_gen_start();
void event_gen_stop();

#endif /* CITY_EVENT_GEN_H_ */
