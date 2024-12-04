/*
 * city_dispatch.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef CITY_DISPATCHER_H_
#define CITY_DISPATCHER_H_

#include <stdio.h>
#include "utils/string_utils.h"
#include "simulation_defs.h"
#include "city_events.h"
#include "city_inbox.h"
#include "city_event_tracker.h"
#include "city_departments.h"
#include "serial_printer.h"

void city_dispatcher_initialize(void);
void city_dispatcher_start();
void city_dispatcher_stop();

#endif /* CITY_DISPATCHER_H_ */
