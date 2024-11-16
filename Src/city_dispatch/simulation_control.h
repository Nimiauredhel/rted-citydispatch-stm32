/*
 * simulation_control.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_CONTROL_H_
#define SIMULATION_CONTROL_H_

#include "cmsis_os.h"

#include "city_dispatcher.h"
#include "simulation_state.h"
#include "event_gen.h"
#include "city_inbox.h"
#include "serial_output.h"

void simulation_start_control_task();

#endif /* SIMULATION_CONTROL_H_ */
