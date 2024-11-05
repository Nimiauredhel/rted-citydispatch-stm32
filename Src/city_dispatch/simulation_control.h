/*
 * simulation_control.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_CONTROL_H_
#define SIMULATION_CONTROL_H_

#include "cmsis_os.h"
#include "simulation_state.h"
#include "simulation_output.h"
#include "event_gen.h"
#include "city_inbox.h"
#include "city_dispatch.h"

void simulation_start_control_task();

#endif /* SIMULATION_CONTROL_H_ */
