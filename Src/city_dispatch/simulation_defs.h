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
#include "utils/string_defs.h"

/* Value defs */
#define NUM_DEPARTMENTS (4)
#define TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define INCOMING_QUEUE_LENGTH (128)
#define OUTPUT_QUEUE_LENGTH (64)
#define DEPARTMENT_QUEUE_LENGTH (32)
#define LOGGER_PRIORITY (50)
#define CENTRAL_DISPATCHER_PRIORITY (100)
#define DEPARTMENT_DISPATCHER_PRIORITY (150)
#define DEPARTMENT_HANDLER_PRIORITY (200)
#define EVENT_GENERATOR_PRIORITY (250)
#define INITIAL_SLEEP (pdMS_TO_TICKS(1000))
#define EVENT_GENERATOR_SLEEP_MAX (pdMS_TO_TICKS(4000))
#define EVENT_GENERATOR_SLEEP_MIN (pdMS_TO_TICKS(2000))
#define LOGGER_SLEEP (pdMS_TO_TICKS(200))

/* Simulation typedefs */
typedef struct CityDepartmentAgentState
{
    bool busy;
    char name[16];
    CityEvent_t currentEvent;
} CityDepartmentAgentState_t;

#endif /* SIMULATION_DEFS_H_ */
