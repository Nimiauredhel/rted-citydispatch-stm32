/*
 * event_config.h
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#ifndef CITY_EVENTS_H_
#define CITY_EVENTS_H_

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "utils/string_defs.h"

#define NUM_JOB_TEMPLATES 12
#define NUM_EVENT_TEMPLATES 8
#define NUM_EVENT_JOBS 4

typedef enum DepartmentCode
{
    DEPT_EMPTY = -1,
    MEDICAL = 0,
    POLICE = 1,
    FIRE = 2,
    COVID = 3
} DepartmentCode_t;
typedef enum JobStatus
{
    // greater than zero: alive, track
    // less than zero: dead, dispose
    JOB_NONE = 0,
    JOB_PENDING = 1,
    JOB_ONGOING = 2,
    JOB_OVERDUE = 3,
    JOB_HANDLED = -1,
    JOB_FAILED = -2,
    JOB_DISMISSED = -3,
} JobStatus_t;
typedef enum EventPriority
{
    EVENT_LOW = 0,
    EVENT_NORMAL = 1,
    EVENT_HIGH = 2
} EventPriority_t;

typedef struct CityJobTemplate
{
    DepartmentCode_t code;
    uint16_t minSecsToHandle;
    uint16_t maxSecsToHandle;
    String30_t description;
} CityJobTemplate_t;

typedef struct CityEventTemplate
{
    EventPriority_t priority;
    uint16_t minSecsToExpire;
    uint16_t maxSecsToExpire;
    String30_t description;
    uint8_t jobTemplateIndices[NUM_EVENT_JOBS];
} CityEventTemplate_t;

typedef struct CityJob
{
	JobStatus_t status;
    DepartmentCode_t code;
    uint16_t secsToHandle;
} CityJob_t;

typedef struct CityEvent
{
	uint8_t eventTemplateIndex;
	RTC_TimeTypeDef expirationTime;
	RTC_DateTypeDef expirationDate;
    CityJob_t jobs[NUM_EVENT_JOBS];
} CityEvent_t;

// Events will be generated, randomly or otherwise,
// from this pool of event templates
extern const CityJobTemplate_t jobTemplates[NUM_JOB_TEMPLATES];
extern const CityEventTemplate_t eventTemplates[NUM_EVENT_TEMPLATES];

#endif /* CITY_EVENTS_H_ */
