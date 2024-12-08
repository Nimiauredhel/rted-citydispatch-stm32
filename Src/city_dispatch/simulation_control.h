/*
 * simulation_control.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_CONTROL_H_
#define SIMULATION_CONTROL_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

#include "city_dispatcher.h"
#include "city_departments.h"
#include "city_event_gen.h"
#include "simulation_state.h"
#include "city_inbox.h"
#include "serial_printer.h"
#include "utils/date_time.h"

void simulation_start_control_task();

#endif /* SIMULATION_CONTROL_H_ */
