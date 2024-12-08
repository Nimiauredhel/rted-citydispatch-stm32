/*
 * city_agents.h
 *
 *  Created on: Dec 4, 2024
 *      Author: mickey
 */

#ifndef CITY_DISPATCH_CITY_AGENTS_H_
#define CITY_DISPATCH_CITY_AGENTS_H_

#include <stdio.h>

#include "serial_printer.h"
#include "simulation_defs.h"
#include "simulation_constants.h"
#include "city_events.h"
#include "city_event_tracker.h"

typedef enum AgentStatus
{
    AGENT_FREE,
    AGENT_ASSIGNED,
    AGENT_BUSY
} AgentStatus_t;

typedef struct AgentState
{
    AgentStatus_t status;
    osMutexId_t mutexHandle;
    osThreadId_t taskHandle;
    uint8_t index;
	CityLog_t log_buffer;
    CityJob_t *currentJob;
    struct AgentState *next;
} AgentState_t;

AgentState_t* city_agents_initialize(uint8_t numOfAgents, DepartmentCode_t code);
void city_agents_start(void);
void city_agents_stop(void);

#endif /* CITY_DISPATCH_CITY_AGENTS_H_ */
