/*
 * city_department.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#include "city_departments.h"

static const uint16_t DISPATCHER_TIMEOUT_MS = 30000;

osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];

const static osMessageQueueAttr_t city_department_inbox_attributes = {
  .name = "cityDeptQueue"
};

const static osThreadAttr_t city_department_task_attributes = {
  .name = "cityDeptTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
};

static DepartmentState_t departments[NUM_DEPARTMENTS] = {0};

static const String30_t msg_task_init =
{
	.size = 28,
	.text = " department task started.\n\r"
};
static const String38_t msg_task_waiting =
{
	.size = 32,
	.text = " department awaiting message.\n\r"
};
static const String30_t msg_task_received =
{
	.size = 29,
	.text = " department received event: "
};

static void city_department_task(void *param);

void city_departments_initialize(void)
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		departments[idx].code = (DepartmentCode_t)idx;
		departments[idx].agentCount = departmentAgentCounts[idx];
		departments[idx].inbox = osMessageQueueNew(DEPARTMENT_QUEUE_LENGTH, sizeof(CityJob_t**), &city_department_inbox_attributes);

		// TODO: create department agents
		/*
		departments[idx].agents = pvPortMalloc(departments[idx].agentCount
				* sizeof(CityDepartmentAgentState_t));
		uint8_t aidx;

		for (aidx = 0; aidx < departments[idx].agentCount; aidx++)
		{

		}*/

		departments[idx].taskHandle = osThreadNew(city_department_task, &departments[idx], &city_department_task_attributes);
		department_inboxes[idx] = &departments[idx].inbox;
		osThreadSuspend(departments[idx].taskHandle);

		serial_printer_spool_chars("Department task initialized.\n\r");
	}
}

void city_departments_start()
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		osThreadResume(departments[idx].taskHandle);
	}
}

void city_departments_stop()
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		osThreadSuspend(departments[idx].taskHandle);
	}
}

static void city_department_task(void *param)
{
	DepartmentState_t *department = (DepartmentState_t *)param;

	serial_printer_spool_chars(departmentNames[department->code]);
	serial_printer_spool_string((String_t *)&msg_task_init);
	osDelay(pdMS_TO_TICKS(1000));

	serial_printer_spool_chars(departmentNames[department->code]);
	serial_printer_spool_string((String_t *)&msg_task_waiting);

	for(;;)
	{
		department->queue_read_status = osMessageQueueGet(department->inbox, &department->current_event_buffer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));

		if (department->queue_read_status == osErrorTimeout)
		{
/*			sprintf(department->output_buffer, " department timed out after %hums\n\r", DISPATCHER_TIMEOUT_MS);
			serial_printer_spool_chars(departmentNames[department->code]);
			serial_printer_spool_chars(department->output_buffer);
			*/
		}
		else if (department->queue_read_status == osOK)
		{
			serial_printer_spool_chars(departmentNames[department->code]);
			serial_printer_spool_string((String_t *)&msg_task_received);
			serial_printer_spool_string((String_t *)&(department->current_event_buffer.description));
			// TODO: assign the event to a resource if one is available
		}
		else
		{
			osDelay(pdMS_TO_TICKS(200));
			continue;
		}

		serial_printer_spool_chars(departmentNames[department->code]);
		serial_printer_spool_string((String_t *)&msg_task_waiting);
	}
}
