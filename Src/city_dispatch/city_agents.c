/*
 * city_agents.c
 *
 *  Created on: Dec 4, 2024
 *      Author: mickey
 */

#include "city_agents.h"

static const uint16_t AGENTS_TIMEOUT_MS = 300;

const osThreadAttr_t city_agent_task_attributes[NUM_DEPARTMENTS] = {
	{ .name = "medicalAgentTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "policeAgentTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "fireAgentTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
	{ .name = "covidAgentTask", .stack_size = TASK_STACK_SIZE, .priority = (osPriority_t) osPriorityNormal, },
};

static const String30_t msg_task_init =
{
	.size = 23,
	.text = " agent task started.\n\r"
};
static const String38_t msg_task_waiting =
{
	.size = 24,
	.text = " agent awaiting work.\n\r"
};
static const String30_t msg_task_received =
{
	.size = 24,
	.text = " agent received job: "
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

        sprintf(newAgents[idx].name, "%s-%u", departmentNames[code], idx+1);
        newAgents[idx].status = AGENT_FREE;
        newAgents[idx].currentJob = NULL;

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
	osDelay(pdMS_TO_TICKS(100));
	AgentState_t *agent = (AgentState_t *)param;

	serial_printer_spool_chars(agent->name);
	serial_printer_spool_string((String_t *)&msg_task_init);
	osDelay(pdMS_TO_TICKS(1000));

	serial_printer_spool_chars(agent->name);
	serial_printer_spool_string((String_t *)&msg_task_waiting);

	for(;;)
	{
		osDelay(pdMS_TO_TICKS(AGENTS_TIMEOUT_MS));
	}
}
