/*
 * city_department.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#include "city_departments.h"

static const uint16_t DISPATCHER_TIMEOUT_MS = 30000;

osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];

const static osMessageQueueAttr_t city_department_inbox_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalDeptQueue" },
	{ .name = "policeDeptQueue" },
	{ .name = "fireDeptQueue" },
	{ .name = "covidDeptQueue" },
};

const static osThreadAttr_t city_department_task_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalDeptTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "policeDeptTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "fireDeptTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "covidDeptTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
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
		departments[idx].inbox = osMessageQueueNew(DEPARTMENT_QUEUE_LENGTH, sizeof(CityJob_t**), &city_department_inbox_attributes[idx]);

		// create department agenst
		departments[idx].agents = city_agents_initialize(departments[idx].agentCount, departments[idx].code);

		departments[idx].taskHandle = osThreadNew(city_department_task, &departments[idx], &city_department_task_attributes[idx]);
		department_inboxes[idx] = &departments[idx].inbox;
		osThreadSuspend(departments[idx].taskHandle);

		serial_printer_spool_chars("Department task initialized.\n\r");
	}
}

void city_departments_start()
{
	uint8_t deptIdx;
	uint8_t agentIdx;

	for (deptIdx = 0; deptIdx < NUM_DEPARTMENTS; deptIdx++)
	{
		osThreadResume(departments[deptIdx].taskHandle);

        for (agentIdx = 0; agentIdx < departments[deptIdx].agentCount; agentIdx++)
        {
            osThreadResume(departments[deptIdx].agents[agentIdx].taskHandle);
        }
	}
}

void city_departments_stop()
{
	uint8_t deptIdx;
	uint8_t agentIdx;

	for (deptIdx = 0; deptIdx < NUM_DEPARTMENTS; deptIdx++)
	{
		osThreadSuspend(departments[deptIdx].taskHandle);

        for (agentIdx = 0; agentIdx < departments[deptIdx].agentCount; agentIdx++)
        {
            osThreadSuspend(departments[deptIdx].agents[agentIdx].taskHandle);
        }
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
		department->queue_read_status = osMessageQueueGet(department->inbox, &department->current_job_pointer, NULL, pdMS_TO_TICKS(DISPATCHER_TIMEOUT_MS));

		if (department->queue_read_status == osOK)
		{
			serial_printer_spool_chars(departmentNames[department->code]);
			serial_printer_spool_string((String_t *)&msg_task_received);
			serial_printer_spool_string((String_t *)&(jobTemplates[department->current_job_pointer->jobTemplateIndex].description));

            bool assigned = false;

            while (!assigned)
            {
				for (uint8_t agentIdx = 0; agentIdx < department->agentCount; agentIdx++)
				{
					if (department->agents[agentIdx].status == AGENT_FREE)
					{
						taskENTER_CRITICAL();
						assigned = true;
						department->agents[agentIdx].currentJob = department->current_job_pointer;
						department->agents[agentIdx].status = AGENT_ASSIGNED;
						department->current_job_pointer = NULL;
						taskEXIT_CRITICAL();
						break;
					}
				}

				osDelay(pdMS_TO_TICKS(100));
            }
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
