/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SERIAL_PRINTER_H_
#define SERIAL_PRINTER_H_

#include "simulation_defs.h"
#include "simulation_constants.h"
#include "utils/serial_output.h"
#include "utils/date_time.h"

typedef enum LogFormat
{
	LOGFMT_INITIALIZING,
	LOGFMT_INITIALIZED,
	LOGFMT_STARTING,
	LOGFMT_STOPPING,
	LOGFMT_WAITING,
	LOGFMT_RECEIVED,
	LOGFMT_PROCESSING,
	LOGFMT_REGISTERED,
	LOGFMT_LOAN_RESOURCE,
	LOGFMT_RETURN_RESOURCE,
	LOGFMT_GENERATING_EVENT,
	LOGFMT_REFRESHING,
	LOGFMT_CLEARED,
	LOGFMT_DONE_WITH,
	LOGFMT_DISMISSING,
} LogFormat_t;

typedef enum LogIdentifier
{
	LOGID_EVENT_GEN,
	LOGID_CITY_INBOX,
	LOGID_DISPATCHER,
	LOGID_EVENT_TRACKER,
	LOGID_DEPARTMENT,
	LOGID_AGENT,
	LOGID_LOGGER,
} LogIdentifier_t;

typedef enum LogSubject
{
	LOGSBJ_EVENT,
	LOGSBJ_JOB,
	LOGSBJ_COMPLETE,
	LOGSBJ_OVERDUE,
	LOGSBJ_DEPRIORITIZED,
	LOGSBJ_INSUFFICIENT_SPACE,
} LogSubject_t;

typedef struct CityLog
{
	LogFormat_t format;

	LogIdentifier_t identifier_0;
	LogIdentifier_t identifier_1;
	LogIdentifier_t identifier_2;

	LogSubject_t subject_0;
	LogSubject_t subject_1;
	LogSubject_t subject_2;

	uint8_t time_hour;
	uint8_t time_min;
	uint8_t time_sec;
} CityLog_t;

void serial_printer_initialize(void);
void serial_printer_spool_log(CityLog_t *new_log);

#endif /* SERIAL_PRINTER_H_ */
