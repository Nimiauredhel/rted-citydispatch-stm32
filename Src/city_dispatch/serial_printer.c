/*
 * simulation_output.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "serial_printer.h"

#define NUM_LOG_FORMATS 23
#define MAX_LEN_LOG_FORMATS 64

/*
 * Private serial printer variables
 */

static const char log_formats[NUM_LOG_FORMATS][MAX_LEN_LOG_FORMATS] =
{
	"~",
	"%s initializing.",
	"%s initialized.",
	"%s task starting.",
	"%s task stopping.",
	"%s awaiting work.",
	"%s received %s: %s.",
	"%s processing %s: %s.",
	"%s registered %s: %s. Index #%u",
	"%s borrowing resource from %s.",
	"%s returning resource to %s.",
	"%s generating %s: %s. Index #%u",
	"%s refreshing.",
	"%s cleared.",
	"%s done with %s: %s. Outcome: %s.",
	"%s removing %s: %s. Cause: %s.",
	"%s dismissing %s: %s. Cause: %s.",
	"%s initializing %s.",
	"%s initialized %s.",
	"%s: starting %s %s. Cause: %s.",
	"%s: stopping %s %s. Cause: %s.",
	"%s: started %s %s following %s.",
	"%s: stopped %s %s following %s.",
};

static const char log_identifiers[10][19] =
{
	"[?]",
	"\b",
	"Event Generator",
	"City Dispatcher",
	"City Inbox",
	"Event Tracker",
	"%s Department",
	"Agent %s-%u",
	"Serial Printer",
	"Simulation Control",
};

static const char log_subjects[10][19] =
{
	"[?]",
	"\b",
	"Simulation",
	"Event",
	"Job",
	"Success",
	"Failure",
	"Deprioritized",
	"Insufficient Space",
	"User Input",
};

static const uint16_t PRINTER_TIMEOUT_MS = 5000;

static const osThreadAttr_t serialPrinterTask_attributes = {
  .name = "serialPrinterTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) LOGGER_PRIORITY,
};

static osThreadId_t serialPrinterTaskHandle;

static osMessageQueueId_t serialPrinterQueueHandle;
static uint8_t serialPrinterQueueBuffer[ OUTPUT_QUEUE_LENGTH * sizeof(CityLog_t) ];
static StaticQueue_t serialPrinterQueueControlBlock;

static const osMessageQueueAttr_t serialPrinterQueue_attributes = {
  .name = "serialPrinterQueue",
  .cb_mem = &serialPrinterQueueControlBlock,
  .cb_size = sizeof(serialPrinterQueueControlBlock),
  .mq_mem = &serialPrinterQueueBuffer,
  .mq_size = sizeof(serialPrinterQueueBuffer)
};

static const char newline[3] = "\n\r";
static const char timestamp_format[18] = "%02u:%02u:%02u ~ ";

static osStatus_t queue_read_status;

static RTC_TimeTypeDef time_buffer;
static CityLog_t log_buffer_incoming;
static CityLog_t log_buffer_outgoing;
static char timestamp_buffer[16];
static char identifier_buffer[32];
static char identifier_buffer_secondary[32];
static char output_buffer[64];

static void serial_printer_task();
static void print_city_log();

void serial_printer_initialize()
{
  serialPrinterQueueHandle = osMessageQueueNew (OUTPUT_QUEUE_LENGTH, sizeof(CityLog_t), &serialPrinterQueue_attributes);
  serialPrinterTaskHandle = osThreadNew(serial_printer_task, NULL, &serialPrinterTask_attributes);
}

void serial_printer_spool_log(CityLog_t *new_log)
{
	time_buffer = time_get();
	log_buffer_incoming = *new_log;
	log_buffer_incoming.time_hour = time_buffer.Hours;
	log_buffer_incoming.time_min = time_buffer.Minutes;
	log_buffer_incoming.time_sec = time_buffer.Seconds;
	osMessageQueuePut(serialPrinterQueueHandle, &log_buffer_incoming, 0, osWaitForever);
}

static void serial_printer_task()
{
	log_buffer_outgoing.format = LOGFMT_TASK_STARTING;
	log_buffer_outgoing.identifier_0 = LOGID_LOGGER;
	serial_printer_spool_log(&log_buffer_outgoing);

	for(;;)
	{
		queue_read_status = osMessageQueueGet(serialPrinterQueueHandle, &log_buffer_outgoing, NULL, pdMS_TO_TICKS(PRINTER_TIMEOUT_MS));

		if (queue_read_status == osErrorTimeout)
		{
			//sprintf(output_buffer, "Printer timed out after %hums\n\r", PRINTER_TIMEOUT_MS);
			//output_print_blocking_autosize(output_buffer);
		}
		else
		{
			print_city_log();
		}
	}
}

void print_city_log()
{
	// prepare timestamp buffer
	sprintf(timestamp_buffer, timestamp_format,
			log_buffer_outgoing.time_hour,
			log_buffer_outgoing.time_min,
			log_buffer_outgoing.time_sec);

	// prepare identifier buffer
	switch (log_buffer_outgoing.identifier_0) {
		case LOGID_DEPARTMENT:
			sprintf(identifier_buffer, log_identifiers[LOGID_DEPARTMENT], departmentNames[log_buffer_outgoing.identifier_1]);
			break;
		case LOGID_AGENT:
			sprintf(identifier_buffer, log_identifiers[LOGID_AGENT], departmentNames[log_buffer_outgoing.identifier_1], log_buffer_outgoing.identifier_2);
			break;
		default:
			sprintf(identifier_buffer, log_identifiers[log_buffer_outgoing.identifier_0]);
			break;
	}

	switch (log_buffer_outgoing.format)
	{
		// log formats that only use the identifier parameters
		case LOGFMT_INITIALIZING:
		case LOGFMT_INITIALIZED:
		case LOGFMT_TASK_STARTING:
		case LOGFMT_TASK_STOPPING:
		case LOGFMT_WAITING:
		case LOGFMT_REFRESHING:
			sprintf(output_buffer, log_formats[log_buffer_outgoing.format], identifier_buffer);
			break;
		// log formats that use every parameter,
		// with subject_2 being a literal index number to be printed
		case LOGFMT_RECEIVED:
		case LOGFMT_PROCESSING:
		case LOGFMT_REGISTERED:
		case LOGFMT_GENERATING_SUBJECT:
			sprintf(output_buffer, log_formats[log_buffer_outgoing.format], identifier_buffer,
					log_subjects[log_buffer_outgoing.subject_0],
					log_buffer_outgoing.subject_0 == LOGSBJ_EVENT ? eventTemplates[log_buffer_outgoing.subject_1].description
						: log_buffer_outgoing.subject_0 == LOGSBJ_JOB ? jobTemplates[log_buffer_outgoing.subject_1].description
						: log_subjects[log_buffer_outgoing.subject_1],
					log_buffer_outgoing.subject_2);
			break;
		// log formats that use every parameter,
		// with subject_2 being an index for a cause/outcome/etc. text
		case LOGFMT_DONE_WITH:
		case LOGFMT_DISMISSING:
		case LOGFMT_STARTING_SUBJECT:
		case LOGFMT_STOPPING_SUBJECT:
		case LOGFMT_STARTED_SUBJECT:
		case LOGFMT_STOPPED_SUBJECT:
			sprintf(output_buffer, log_formats[log_buffer_outgoing.format], identifier_buffer,
					log_subjects[log_buffer_outgoing.subject_0],
					log_buffer_outgoing.subject_0 == LOGSBJ_EVENT ? eventTemplates[log_buffer_outgoing.subject_1].description
						: log_buffer_outgoing.subject_0 == LOGSBJ_JOB ? jobTemplates[log_buffer_outgoing.subject_1].description
						: log_subjects[log_buffer_outgoing.subject_1],
					log_subjects[log_buffer_outgoing.subject_2]);
			break;
		// log formats that use the subject fields like a secondary identifier
		// to show interactions between departments, agents etc.
		case LOGFMT_INITIALIZING_SUBJECT:
		case LOGFMT_INITIALIZED_SUBJECT:
		case LOGFMT_BORROWING_RESOURCE:
		case LOGFMT_RETURNING_RESOURCE:
			// prepare secondary identifier buffer
			switch (log_buffer_outgoing.subject_0)
			{
				case LOGID_DEPARTMENT:
					sprintf(identifier_buffer_secondary, log_identifiers[LOGID_DEPARTMENT], departmentNames[log_buffer_outgoing.subject_1]);
					break;
				case LOGID_AGENT:
					sprintf(identifier_buffer_secondary, log_identifiers[LOGID_AGENT], departmentNames[log_buffer_outgoing.subject_1], log_buffer_outgoing.subject_2);
					break;
				default:
					sprintf(identifier_buffer_secondary, log_identifiers[log_buffer_outgoing.subject_0]);
					break;
			}

			// apply both identifier buffers
			sprintf(output_buffer, log_formats[log_buffer_outgoing.format],
					identifier_buffer,
					identifier_buffer_secondary);
			break;

		default:
	}

	output_print_blocking(timestamp_buffer, 12);
	output_print_blocking_autosize(output_buffer);
	output_print_blocking(newline, 3);
}
