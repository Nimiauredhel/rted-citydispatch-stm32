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
		serial_printer_spool_log(&newAgents[idx].log_buffer);

        newAgents[idx].taskHandle = osThreadNew(city_agent_task, &newAgents[idx], &city_agent_task_attributes[code]);
        osThreadSuspend(newAgents[idx].taskHandle);
		osDelay(DELAY_10MS_TICKS);
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
	serial_printer_spool_log(&agent->log_buffer);

	//osDelay(pdMS_TO_TICKS(100));
	//agent->log_buffer.format = LOGFMT_WAITING;
	//serial_printer_spool_log(&agent->log_buffer);

	for(;;)
	{
		osDelay(DELAY_100MS_TICKS);

		if (agent->status == AGENT_ASSIGNED)
		{
			taskENTER_CRITICAL();

			if (agent->currentJob->status == JOB_PENDING)
			{
				agent->status = AGENT_BUSY;
				agent->currentJob->status = JOB_ONGOING;

				agent->log_buffer.format = LOGFMT_RECEIVED;
				agent->log_buffer.subject_0 = LOGSBJ_JOB;
				agent->log_buffer.subject_1 = agent->currentJob->jobTemplateIndex;
				serial_printer_spool_log(&agent->log_buffer);
				// TODO: think about whether this action also requires
				// setting the event tracker to dirty
			}
			else if (agent->currentJob->status == JOB_OVERDUE)
			{
				agent->currentJob->status = JOB_FAILED;
				event_tracker_set_dirty();
			}

			taskEXIT_CRITICAL();

			// wait the job handling duration
			osDelay(pdMS_TO_TICKS(agent->currentJob->secsToHandle * 1000));

			// TODO: improve the job status handling between the agent and tracker
			// TODO: separate agent<->tracker interface from dispatcher<->tracker interface?

			taskENTER_CRITICAL();

			agent->log_buffer.format = LOGFMT_DONE_WITH;
			agent->log_buffer.subject_0 = LOGSBJ_JOB;
			agent->log_buffer.subject_1 = agent->currentJob->jobTemplateIndex;

			if (agent->currentJob->status == JOB_OVERDUE)
			{
				agent->currentJob->status = JOB_FAILED;
				agent->log_buffer.subject_2 = LOGSBJ_OVERDUE;
			}
			else
			{
				agent->currentJob->status = JOB_HANDLED;
				agent->log_buffer.subject_2 = LOGSBJ_COMPLETE;
			}

			agent->status = AGENT_FREE;
			agent->currentJob = NULL;
			event_tracker_set_dirty();

			taskEXIT_CRITICAL();

			serial_printer_spool_log(&agent->log_buffer);
		}
	}
}
