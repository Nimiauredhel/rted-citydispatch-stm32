/*
 * simulation_output.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "serial_printer.h"

#define NUM_LOG_FORMATS 15
#define MAX_LEN_LOG_FORMATS 64

/*
 * Private serial printer variables
 */

static const char log_formats[NUM_LOG_FORMATS][MAX_LEN_LOG_FORMATS] =
{
	"Initializing %s.",
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
	"%s done with %s: %s.\n\rOutcome: %s.",
	"%s dismissing %s: %s.\n\rCause: %s.",
};

static const char log_identifiers[7][16] =
{
	"Event Generator",
	"City Dispatcher",
	"City Inbox",
	"Event Tracker",
	"%s Department",
	"Agent %s-%u",
	"Logger",
};

static const char log_subjects[6][19] =
{
	"event",
	"job",
	"success",
	"failure",
	"deprioritized",
	"insufficient space",
};

static const uint16_t PRINTER_TIMEOUT_MS = 5000;

static const osThreadAttr_t serialPrinterTask_attributes = {
  .name = "serialPrinterTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
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

static osStatus_t queue_read_status;

static CityLog_t log_buffer;
static char identifier_buffer[32];
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
	osMessageQueuePut(serialPrinterQueueHandle, new_log, 0, osWaitForever);
}

static void serial_printer_task()
{
	log_buffer.format = LOGFMT_STARTING;
	log_buffer.identifier_0 = LOGID_LOGGER;
	serial_printer_spool_log(&log_buffer);

	for(;;)
	{
		queue_read_status = osMessageQueueGet(serialPrinterQueueHandle, &log_buffer, NULL, pdMS_TO_TICKS(PRINTER_TIMEOUT_MS));

		if (queue_read_status == osErrorTimeout)
		{
			//sprintf(output_buffer, "Printer timed out after %hums\n\r", PRINTER_TIMEOUT_MS);
			//output_print_blocking_autosize(output_buffer);
		}
		else
		{
			print_city_log(log_buffer);
		}
	}
}

void print_city_log(CityLog_t log)
{
	// prepare identifier buffer
	switch (log.identifier_0) {
		case LOGID_DEPARTMENT:
			sprintf(identifier_buffer, log_identifiers[LOGID_DEPARTMENT], departmentNames[log.identifier_1]);
			break;
		case LOGID_AGENT:
			sprintf(identifier_buffer, log_identifiers[LOGID_AGENT], departmentNames[log.identifier_1], log.identifier_2);
			break;
		default:
			sprintf(identifier_buffer, log_identifiers[log.identifier_0]);
			break;
	}

	switch (log.format)
	{
	// first address the exceptions
		case LOGFMT_INITIALIZING:
		case LOGFMT_INITIALIZED:
		case LOGFMT_STARTING:
		case LOGFMT_STOPPING:
		case LOGFMT_WAITING:
		case LOGFMT_REFRESHING:
			sprintf(output_buffer, log_formats[log.format], identifier_buffer);
			break;
		case LOGFMT_RECEIVED:
		case LOGFMT_PROCESSING:
		case LOGFMT_REGISTERED:
		case LOGFMT_GENERATING_EVENT:
			sprintf(output_buffer, log_formats[log.format], identifier_buffer, log_subjects[log.subject_0],
					log.subject_0 == LOGSBJ_EVENT ? eventTemplates[log.subject_1].description
					: jobTemplates[log.subject_1].description,
					 log.subject_2);
			break;
		case LOGFMT_DONE_WITH:
		case LOGFMT_DISMISSING:
			sprintf(output_buffer, log_formats[log.format], identifier_buffer, log_subjects[log.subject_0],
					log.subject_0 == LOGSBJ_EVENT ? eventTemplates[log.subject_1].description
					: jobTemplates[log.subject_1].description,
					  log_subjects[log.subject_2]);
			break;
		case LOGFMT_LOAN_RESOURCE:
		case LOGFMT_RETURN_RESOURCE:
			break;

		default:
	}

	output_print_blocking_autosize(output_buffer);
	output_print_blocking(newline, 3);
}
