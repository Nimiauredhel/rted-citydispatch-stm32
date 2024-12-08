/*
 * city_agents.c
 *
 *  Created on: Dec 4, 2024
 *      Author: mickey
 */

#include "city_agents.h"

const osThreadAttr_t city_agent_task_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalAgentTask", .stack_size = AGENT_TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "policeAgentTask", .stack_size = AGENT_TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "fireAgentTask", .stack_size = AGENT_TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "covidAgentTask", .stack_size = AGENT_TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
};

static AgentState_t *head = NULL;
static AgentState_t *tail = NULL;

static void city_agent_task(void *param);

AgentState_t* city_agents_initialize(uint8_t numOfAgents, DepartmentCode_t code)
{
	uint8_t idx;
    AgentState_t *newAgents = pvPortMalloc(sizeof(AgentState_t) * numOfAgents);

	for (idx = 0; idx < numOfAgents; idx++)
	{
        if (head == NULL)
        {
            head = &newAgents[idx];
            tail = &newAgents[idx];
        }
        else
        {
            tail->next = &newAgents[idx];
            tail = &newAgents[idx];
        }

        newAgents[idx].index = idx;
        newAgents[idx].status = AGENT_FREE;
        newAgents[idx].currentJob = NULL;

		newAgents[idx].log_buffer.identifier_0 = LOGID_AGENT;
		newAgents[idx].log_buffer.identifier_1 = code;
		newAgents[idx].log_buffer.identifier_2 = idx;
        newAgents[idx].log_buffer.format = LOGFMT_INITIALIZED;
		serial_printer_spool_log(newAgents[idx].log_buffer);

        newAgents[idx].taskHandle = osThreadNew(city_agent_task, &newAgents[idx], &city_agent_task_attributes[code]);
        osThreadSuspend(newAgents[idx].taskHandle);
	}

	return newAgents;
}

void city_agents_start()
{
    if (head == NULL) return;

    AgentState_t *current = head;

    do
    {
        osThreadResume(current->taskHandle);
        current = current->next;
    } while (current != NULL);
}

void city_agents_stop()
{
    if (head == NULL) return;

    AgentState_t *current = head;

    do
    {
        osThreadSuspend(current->taskHandle);
        current = current->next;
    } while (current != NULL);
}

static void city_agent_task(void *param)
{
	osDelay(DELAY_100MS_TICKS);
	AgentState_t *agent = (AgentState_t *)param;

	agent->log_buffer.format = LOGFMT_TASK_STARTING;
	serial_printer_spool_log(agent->log_buffer);

	for(;;)
	{
		while (agent->status == AGENT_FREE
				|| agent->currentJob == NULL)
		{
			osDelay(DELAY_100MS_TICKS);
		}

		if (agent->currentJob->status == JOB_PENDING)
		{
			osMutexAcquire(eventTrackerMutexHandle, osWaitForever);

			agent->status = AGENT_BUSY;
			agent->currentJob->status = JOB_ONGOING;

			agent->log_buffer.format = LOGFMT_RECEIVED;
			agent->log_buffer.subject_0 = LOGSBJ_JOB;
			agent->log_buffer.subject_1 = agent->currentJob->jobTemplateIndex;

			osMutexRelease(eventTrackerMutexHandle);
			serial_printer_spool_log(agent->log_buffer);

			// wait the job handling duration
			osDelay(pdMS_TO_TICKS(agent->currentJob->secsToHandle * 1000));
		}
		else if (agent->currentJob->status == JOB_OVERDUE)
		{
			agent->currentJob->status = JOB_FAILED;
			event_tracker_set_dirty();
		}

		// TODO: improve the job status handling between the agent and tracker
		// TODO: separate agent<->tracker interface from dispatcher<->tracker interface?

		osMutexAcquire(eventTrackerMutexHandle, osWaitForever);

		agent->log_buffer.format = LOGFMT_DONE_WITH;
		agent->log_buffer.subject_0 = LOGSBJ_JOB;
		agent->log_buffer.subject_1 = agent->currentJob->jobTemplateIndex;

		if (agent->currentJob->status == JOB_OVERDUE)
		{
			agent->currentJob->status = JOB_FAILED;
			agent->log_buffer.subject_2 = LOGSBJ_FAILURE;
		}
		else
		{
			agent->currentJob->status = JOB_HANDLED;
			agent->log_buffer.subject_2 = LOGSBJ_SUCCESS;
		}

		agent->status = AGENT_FREE;
		agent->currentJob = NULL;

		osMutexRelease(eventTrackerMutexHandle);

		event_tracker_set_dirty();
		serial_printer_spool_log(agent->log_buffer);
	}
}
