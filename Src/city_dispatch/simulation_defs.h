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

/* Value defs */
#define NUM_DEPARTMENTS (4)
#define TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define INCOMING_QUEUE_LENGTH (256)
#define DEPARTMENT_QUEUE_LENGTH (256)
#define LOGGER_PRIORITY (50)
#define CENTRAL_DISPATCHER_PRIORITY (100)
#define DEPARTMENT_DISPATCHER_PRIORITY (150)
#define DEPARTMENT_HANDLER_PRIORITY (200)
#define EVENT_GENERATOR_PRIORITY (250)
#define INITIAL_SLEEP (pdMS_TO_TICKS(1000))
#define EVENT_GENERATOR_SLEEP_MAX (pdMS_TO_TICKS(6000))
#define EVENT_GENERATOR_SLEEP_MIN (pdMS_TO_TICKS(2000))
#define LOGGER_SLEEP (pdMS_TO_TICKS(200))

/* Type defs */
typedef enum DepartmentCode
{
    MEDICAL = 0,
    POLICE = 1,
    FIRE = 2,
    COVID = 3
} DepartmentCode_t;
typedef struct CityEvent
{
    TickType_t ticks;
    DepartmentCode_t code;
    char *description;
} CityEvent_t;
typedef struct CityDepartmentAgentState
{
    bool busy;
    char name[16];
    CityEvent_t currentEvent;
} CityDepartmentAgentState_t;
typedef struct CityDepartment
{
    DepartmentCode_t code;
    BaseType_t status;
    StaticQueue_t jobQueue;
    uint8_t agentCount;
    CityDepartmentAgentState_t *agentStates;
} CityDepartment_t;
typedef struct CityData
{
    BaseType_t dispatcherStatus;
    StaticQueue_t incomingQueue;
    CityDepartment_t departments[NUM_DEPARTMENTS];
} CityData_t;
typedef struct CityEventTemplate
{
    TickType_t minTicks;
    TickType_t maxTicks;
    DepartmentCode_t code;
    char *description;
} CityEventTemplate_t;
#endif /* SIMULATION_DEFS_H_ */
