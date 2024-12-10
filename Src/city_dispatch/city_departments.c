/*
 * city_department.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

/** @addtogroup City_Dispatch
  * @{
  */

/**
 * @file city_departments.c
 * @brief Implements the "City Department" task, which is run in multiple instances.
 * A city department is associated with several resources ("Agent" tasks), and the
 * department task's job is to assign incoming work to these agents as it comes in.
 */

#include "city_departments.h"

DepartmentLoad_t department_loads[NUM_DEPARTMENTS];
osMessageQueueId_t *department_inboxes[NUM_DEPARTMENTS];

const static osMessageQueueAttr_t city_department_inbox_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalDeptQueue" },
	{ .name = "policeDeptQueue" },
	{ .name = "fireDeptQueue" },
	{ .name = "covidDeptQueue" },
};

const static osThreadAttr_t city_department_task_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalDeptTask", .stack_size = DEPARTMENT_TASK_STACK_SIZE, .priority = (osPriority_t) CITY_DEPARTMENT_PRIORITY, },
	{ .name = "policeDeptTask", .stack_size = DEPARTMENT_TASK_STACK_SIZE, .priority = (osPriority_t) CITY_DEPARTMENT_PRIORITY, },
	{ .name = "fireDeptTask", .stack_size = DEPARTMENT_TASK_STACK_SIZE, .priority = (osPriority_t) CITY_DEPARTMENT_PRIORITY, },
	{ .name = "covidDeptTask", .stack_size = DEPARTMENT_TASK_STACK_SIZE, .priority = (osPriority_t) CITY_DEPARTMENT_PRIORITY, },
};

static DepartmentState_t departments[NUM_DEPARTMENTS] = {0};

static void city_department_task(void *param);

void city_departments_initialize(void)
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		departments[idx].log_buffer.identifier_0 = LOGID_DEPARTMENT;
		departments[idx].log_buffer.identifier_1 = idx;
		departments[idx].log_buffer.format = LOGFMT_INITIALIZING;
		serial_printer_spool_log(departments[idx].log_buffer);

		departments[idx].code = (DepartmentCode_t)idx;
		departments[idx].agentCount = departmentAgentCounts[idx];
		departments[idx].inbox = osMessageQueueNew(DEPARTMENT_QUEUE_LENGTH, sizeof(CityJob_t**), &city_department_inbox_attributes[idx]);

		// create department agents
		departments[idx].agents = city_agents_initialize(departments[idx].agentCount, departments[idx].code);
		department_inboxes[idx] = &departments[idx].inbox;
		department_loads[idx] = DEPARTMENT_LOAD_FREE;

		departments[idx].log_buffer.format = LOGFMT_INITIALIZED;
		serial_printer_spool_log(departments[idx].log_buffer);

		departments[idx].taskHandle = osThreadNew(city_department_task, &departments[idx], &city_department_task_attributes[idx]);
		osThreadSuspend(departments[idx].taskHandle);
		osDelay(DELAY_100MS_TICKS);
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
	osDelay(DELAY_100MS_TICKS);
	DepartmentState_t *department = (DepartmentState_t *)param;
	department->log_buffer.identifier_0 = LOGID_DEPARTMENT;
	department->log_buffer.identifier_1 = department->code;

	department->log_buffer.format = LOGFMT_TASK_STARTING;
	serial_printer_spool_log(department->log_buffer);

	//osDelay(pdMS_TO_TICKS(100));
	//department->log_buffer.format = LOGFMT_WAITING;
	//serial_printer_spool_log(&department->log_buffer);

	for(;;)
	{
		department->queue_read_status = osMessageQueueGet(department->inbox, &department->current_job_pointer, NULL, osWaitForever);

		if (department->queue_read_status == osOK)
		{
			department->log_buffer.format = LOGFMT_RECEIVED;
			department->log_buffer.subject_0 = LOGSBJ_JOB;
			department->log_buffer.subject_1 = department->current_job_pointer->jobTemplateIndex;
			serial_printer_spool_log(department->log_buffer);

            bool assigned = false;
            osStatus_t agentMutexStatus;

            while (!assigned)
            {
				osDelay(DELAY_10MS_TICKS);

				if (department_loads[department->code] < DEPARTMENT_LOAD_MAX)
				{
					department_loads[department->code]++;
				}

				for (uint8_t agentIdx = 0; agentIdx < department->agentCount; agentIdx++)
				{
					agentMutexStatus = osMutexAcquire(department->agents[agentIdx].mutexHandle, DELAY_100MS_TICKS);
					if (agentMutexStatus != osOK) continue;

					if (department->agents[agentIdx].status == AGENT_FREE)
					{
						assigned = true;
						department->agents[agentIdx].currentJob = department->current_job_pointer;
						department->agents[agentIdx].status = AGENT_ASSIGNED;
						department->current_job_pointer->assignedAgentMutex = department->agents[agentIdx].mutexHandle;
						department->current_job_pointer = NULL;
						osMutexRelease(department->agents[agentIdx].mutexHandle);
						break;
					}

					osMutexRelease(department->agents[agentIdx].mutexHandle);
				}
            }
		}
	}
}

/** 
  * @}
  */
