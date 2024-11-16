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
#define OUTPUT_QUEUE_LENGTH (64)
#define DEPARTMENT_QUEUE_LENGTH (256)
#define LOGGER_PRIORITY (50)
#define CENTRAL_DISPATCHER_PRIORITY (100)
#define DEPARTMENT_DISPATCHER_PRIORITY (150)
#define DEPARTMENT_HANDLER_PRIORITY (200)
#define EVENT_GENERATOR_PRIORITY (250)
#define INITIAL_SLEEP (pdMS_TO_TICKS(1000))
#define EVENT_GENERATOR_SLEEP_MAX (pdMS_TO_TICKS(4000))
#define EVENT_GENERATOR_SLEEP_MIN (pdMS_TO_TICKS(2000))
#define LOGGER_SLEEP (pdMS_TO_TICKS(200))

/* Hard-coded structured string structs.
 * felt smart might delete later
 */
typedef struct String
{
	uint8_t size;
	char text[];
} String_t;
typedef struct String6
{
	uint8_t size;
	char text[7];
} String6_t;
typedef struct String14
{
	uint8_t size;
	char text[15];
} String14_t;
typedef struct String22
{
	uint8_t size;
	char text[23];
} String22_t;
typedef struct String30
{
	uint8_t size;
	char text[31];
} String30_t;
typedef struct String38
{
	uint8_t size;
	char text[39];
} String38_t;
typedef struct String46
{
	uint8_t size;
	char text[47];
} String46_t;
typedef struct String54
{
	uint8_t size;
	char text[55];
} String54_t;
typedef struct String62
{
	uint8_t size;
	char text[63];
} String62_t;

/* Simulation type defs */
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
    String46_t description;
} CityEvent_t;
typedef struct CityDepartmentAgentState
{
    bool busy;
    char name[16];
    CityEvent_t currentEvent;
} CityDepartmentAgentState_t;
typedef struct CityEventTemplate
{
    TickType_t minTicks;
    TickType_t maxTicks;
    DepartmentCode_t code;
    char *description;
} CityEventTemplate_t;


#endif /* SIMULATION_DEFS_H_ */
