/*
 *
 *  Created on: Dec 3, 2024
 *      Author: mickey
 */

#include "city_event_tracker.h"

static const char dismissal_reasons[4][17] =
{
	"Not Dismissed.\n\r",
	"Success.\n\r",
	"Deprioritized.\n\r",
	"Failure.\n\r",
};

static uint8_t length = 0;
static uint8_t nextFreeIdx = 0;
static EventTrackerNode_t *head;
static EventTrackerNode_t *tail;
static EventTrackerNode_t nodeBuffer[EVENT_TRACKER_CAPACITY] = {0};

static void set_next_free_idx()
{
    if (length == 0)
    {
        nextFreeIdx = 0;
        return;
    }

    uint8_t idx = 0;

    for (idx = 0; idx < EVENT_TRACKER_CAPACITY; idx++)
    {
        if (!nodeBuffer[idx].used)
        {
            nextFreeIdx = idx;
            return;
        }
    }
}

void event_tracker_initialize()
{
    head = NULL;
    tail = NULL;
    length = 0;
    nextFreeIdx = 0;

    uint8_t idx = 0;

    for (idx = 0; idx < EVENT_TRACKER_CAPACITY; idx++)
    {
        nodeBuffer[idx].storageIdx = idx;
        nodeBuffer[idx].next = NULL;
        nodeBuffer[idx].used = false;
    }

	serial_printer_spool_chars("Event Tracker initialized.\n\r");
}

// if successful, returns pointer to stored address
// if failed, returns NULL
CityEvent_t *event_tracker_add(CityEvent_t newEvent)
{
    if (length >= EVENT_TRACKER_CAPACITY)
    {
		serial_printer_spool_chars("Event Tracker failed to add event.\n\r");
        return NULL;
    }

    serial_printer_spool_chars("Event Tracker adding event.\n\r");

    EventTrackerNode_t *targetNode;

    if (length == 0)
    {
        targetNode = &nodeBuffer[0];
        head = targetNode;
    }
    else
    {
        targetNode = &nodeBuffer[nextFreeIdx];
        tail->next = targetNode;
    }

    targetNode->event = newEvent;
    targetNode->next = NULL;
    tail = targetNode;

    length++;

    if (length < EVENT_TRACKER_CAPACITY)
    {
        set_next_free_idx();
    }

    return &(targetNode->event);
}

void event_tracker_refresh()
{
    if (length == 0) return;

    serial_printer_spool_chars("Event Tracker Refresh.\n\r");

    // this function invokes CRITICAL mode
    // due to "job status" being used as IPC
    // between dispatcher & agents.
    // TODO: use mutex instead (to only lock individual resources when required, rather than whole system every time)
    taskENTER_CRITICAL();

    uint8_t dismissed = 0;
    uint8_t jobIdx = 0;
    EventTrackerNode_t *current = head;
    EventTrackerNode_t *prev = NULL;

    do
    {
        dismissed = 0;

        for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
        {
            if (current->event.jobs[jobIdx].status >= 0)
            {
            	continue;
            }
            else
            {
				if (current->event.jobs[jobIdx].status == JOB_FAILED)
				{
					dismissed = 3;
					break;
				}

				if (dismissed < 2 && current->event.jobs[jobIdx].status == JOB_DISMISSED)
				{
					dismissed = 2;
				}
				else if (dismissed < 1 && current->event.jobs[jobIdx].status == JOB_HANDLED)
				{
					dismissed = 1;
				}
            }
        }

        if (dismissed == 0)
        {
            prev = current;
        }
        else
        {
			serial_printer_spool_chars("Event Tracker: freeing event ");
			serial_printer_spool_chars(&eventTemplates[current->event.eventTemplateIndex].description);
			serial_printer_spool_chars("Cause: ");
			serial_printer_spool_chars(dismissal_reasons[dismissed]);
            // freeing a node
            current->used = false;
            length--;

            // linked list stuff
            // if this is the head, move forward the head
            if (prev == NULL)
            {
                head = current->next;
            }
            // if this is the tail, move back the tail
            if (current->next == NULL)
            {
                tail = prev;
            }
            else if (prev != NULL)
            {
                prev->next = current->next;
            }
        }

        current = current->next;
    } while (current != NULL);

    taskEXIT_CRITICAL();
}

uint8_t event_tracker_get_remaining_storage()
{
    return (EVENT_TRACKER_CAPACITY - length);
}

uint8_t event_tracker_get_length()
{
    return length;
}
