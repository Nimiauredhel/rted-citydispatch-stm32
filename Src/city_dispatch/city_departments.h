/*
 * city_department.h
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

/** @addtogroup City_Dispatch
  * @{
  */

#ifndef CITY_DEPARTMENTS_H_
#define CITY_DEPARTMENTS_H_

#include <stdio.h>

#include "serial_printer.h"
#include "simulation_defs.h"
#include "simulation_constants.h"
#include "city_agents.h"

/*
 * Department typedefs
 */

typedef struct DepartmentState
{
	osStatus_t queue_read_status;
	CityJob_t *current_job_pointer;
    DepartmentCode_t code;
    osThreadId_t taskHandle;
    osMessageQueueId_t inbox;
    uint8_t agentCount;
    CityLog_t log_buffer;
    struct AgentState *agents;
} DepartmentState_t;

typedef enum DepartmentLoad
{
	DEPARTMENT_LOAD_FREE = 0,
	DEPARTMENT_LOAD_LOW = 1,
	DEPARTMENT_LOAD_HIGH = 2,
	DEPARTMENT_LOAD_MAX = 3,
} DepartmentLoad_t;

/*
 * Department variables
 */
extern osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];
extern DepartmentLoad_t department_loads[NUM_DEPARTMENTS];

void city_departments_initialize(void);
void city_departments_start(void);
void city_departments_stop(void);

#endif /* CITY_DEPARTMENTS_H_ */

/** 
  * @}
  */
