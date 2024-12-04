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

void event_gen_initialize();
void event_gen_start();
void event_gen_stop();

#endif /* CITY_EVENT_GEN_H_ */
