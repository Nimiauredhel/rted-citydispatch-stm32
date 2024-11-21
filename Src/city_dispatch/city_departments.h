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

typedef struct DepartmentState
{
    DepartmentCode_t code;
    BaseType_t status;
    osMessageQueueId_t inbox;
    uint8_t agentCount;
    CityDepartmentAgentState_t *agentStates;
} DepartmentState_t;

void city_departments_initialize(void);

#endif /* CITY_DEPARTMENTS_H_ */
