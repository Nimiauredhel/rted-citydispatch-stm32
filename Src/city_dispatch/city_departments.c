/*
 * city_department.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#include "city_departments.h"

osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];

const static osMessageQueueAttr_t city_department_inbox_attributes = {
  .name = "cityDeptQueue"
};

const static osThreadAttr_t city_department_task_attributes = {
  .name = "cityDeptTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
};

static DepartmentInfo_t departments[NUM_DEPARTMENTS] = {0};

static void city_department_task(void *param);

void city_departments_initialize(void)
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		departments[idx].code = (DepartmentCode_t)idx;
		departments[idx].agentCount = departmentAgentCounts[idx];
		departments[idx].inbox = osMessageQueueNew(DEPARTMENT_QUEUE_LENGTH, sizeof(CityEvent_t), &city_department_inbox_attributes);

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
	DepartmentInfo_t *info = (DepartmentInfo_t *)param;

	serial_printer_spool_chars(departmentNames[info->code]);
	serial_printer_spool_chars(" Department task started.\n\r");

	for(;;)
	{
		osDelay(1000);
		serial_printer_spool_chars(departmentNames[info->code]);
		serial_printer_spool_chars(" Department task hello.\n\r");
	}
}
