/*
 * event_config.h
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

/** @addtogroup City_Dispatch
  * @{
  */

#ifndef CITY_EVENTS_H_
#define CITY_EVENTS_H_

#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

#define NUM_JOB_TEMPLATES 12
#define NUM_EVENT_TEMPLATES 9
#define NUM_EVENT_JOBS 8

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
    JOB_FAILED_TRACKER = 3,
    JOB_CANCELLED_TRACKER = 4,
    JOB_HANDLED_AGENT = -1,
    JOB_FAILED_AGENT = -2,
    JOB_CANCELLED_AGENT = -3,
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
    char description[32];
} CityJobTemplate_t;

typedef struct CityEventTemplate
{
    EventPriority_t priority;
    uint16_t minSecsToExpire;
    uint16_t maxSecsToExpire;
    char description[32];
    uint8_t jobTemplateIndices[NUM_EVENT_JOBS];
} CityEventTemplate_t;

typedef struct CityJob
{
    uint8_t jobTemplateIndex;
	JobStatus_t status;
    DepartmentCode_t code;
    uint16_t secsToHandle;
    osMutexId_t *assignedAgentMutex;
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

/** 
  * @}
  */
