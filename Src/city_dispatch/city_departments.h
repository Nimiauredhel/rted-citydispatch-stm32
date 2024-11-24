/*
 * city_department.h
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#ifndef CITY_DEPARTMENTS_H_
#define CITY_DEPARTMENTS_H_

#include <stdio.h>

#include "serial_printer.h"
#include "simulation_defs.h"

/*
 * Department typedefs
 */
typedef struct DepartmentState
{
    DepartmentCode_t code;
    BaseType_t status;
    osMessageQueueId_t inbox;
    uint8_t agentCount;
    CityDepartmentAgentState_t *agentStates;
} DepartmentState_t;

/*
 * Department variables
 */
osThreadId_t department_tasks[NUM_DEPARTMENTS];
osMessageQueueId_t department_inboxes[NUM_DEPARTMENTS];

void city_departments_initialize(void);
void city_departments_start(void);
void city_departments_stop(void);

#endif /* CITY_DEPARTMENTS_H_ */
