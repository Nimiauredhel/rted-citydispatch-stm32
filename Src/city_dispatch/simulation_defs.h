/*
 * simulation_defs.h
 *
 *  Created on: Nov 12, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_DEFS_H_
#define SIMULATION_DEFS_H_

#include <stdbool.h>
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

#include "city_events.h"

/* Value defs */
// TODO: set correct priorities with CMSIS enums
// TODO: remove unused defines
#define NUM_DEPARTMENTS (4)
#define TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define EVENT_TRACKER_CAPACITY (16)
#define INCOMING_QUEUE_LENGTH (32)
#define OUTPUT_QUEUE_LENGTH (64)
#define DEPARTMENT_QUEUE_LENGTH (8)
#define LOGGER_PRIORITY (24)
#define CENTRAL_DISPATCHER_PRIORITY (24)
#define DEPARTMENT_DISPATCHER_PRIORITY (24)
#define DEPARTMENT_HANDLER_PRIORITY (24)
#define EVENT_GENERATOR_PRIORITY (24)
#define EVENT_GENERATOR_SLEEP_MAX (4000)
#define EVENT_GENERATOR_SLEEP_MIN (2000)

/* Simulation typedefs */
typedef struct CityDepartmentAgentState
{
    bool busy;
    char name[16];
    CityEvent_t currentEvent;
} CityDepartmentAgentState_t;

#endif /* SIMULATION_DEFS_H_ */
