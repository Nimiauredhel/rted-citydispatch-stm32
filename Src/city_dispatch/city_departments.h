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
#include "simulation_constants.h"

/*
 * Department typedefs
 */

typedef struct DepartmentInfo
{
    DepartmentCode_t code;
    osThreadId_t taskHandle;
    osMessageQueueId_t inbox;
    uint8_t agentCount;
    CityDepartmentAgentState_t *agents;
} DepartmentInfo_t;

typedef struct DepartmentState
{
    BaseType_t status;
} DepartmentState_t;

/*
 * Department variables
 */
extern osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];

void city_departments_initialize(void);
void city_departments_start(void);
void city_departments_stop(void);

#endif /* CITY_DEPARTMENTS_H_ */
