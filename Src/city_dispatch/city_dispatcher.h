/*
 * city_dispatch.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef CITY_DISPATCHER_H_
#define CITY_DISPATCHER_H_

#include <stdio.h>
#include "simulation_defs.h"
#include "simulation_output.h"
#include "city_inbox.h"

typedef struct DispatcherState
{
	osStatus_t queue_read_status;
	CityEvent_t current_event_buffer;
	char output_buffer[48];
} DispatcherState_t;

void city_dispatcher_initialize(void);

#endif /* CITY_DISPATCHER_H_ */
