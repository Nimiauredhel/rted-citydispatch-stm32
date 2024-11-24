/*
 * city_department.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#include "city_departments.h"

const static osMessageQueueAttr_t city_department_inbox_attributes = {
  .name = "dynamicQueue"
};

const static osThreadAttr_t city_department_task_attributes = {
  .name = "cityDeptTask",
  .stack_size = TASK_STACK_SIZE,
  .priority = (osPriority_t) DEPARTMENT_DISPATCHER_PRIORITY,
};

osThreadId_t department_tasks[NUM_DEPARTMENTS] = {0};
osMessageQueueId_t department_inboxes[NUM_DEPARTMENTS] = {0};

static void city_department_task(void *param);

void city_departments_initialize(void)
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		department_inboxes[idx] = osMessageQueueNew(DEPARTMENT_QUEUE_LENGTH, sizeof(CityEvent_t), &city_department_inbox_attributes);
		department_tasks[idx] = osThreadNew(city_department_task, &department_inboxes[idx], &city_department_task_attributes);
		osThreadSuspend(department_tasks[idx]);
	}
}

void city_departments_start()
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		osThreadResume(department_tasks[idx]);
	}
}

void city_departments_stop()
{
	uint8_t idx;

	for (idx = 0; idx < NUM_DEPARTMENTS; idx++)
	{
		osThreadSuspend(department_tasks[idx]);
	}
}

static void city_department_task(void *param)
{
}
