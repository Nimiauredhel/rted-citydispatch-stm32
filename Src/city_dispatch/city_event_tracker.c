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

static int8_t length = 0;
static int8_t nextFreeIdx = 0;
static int8_t headIdx;
static int8_t tailIdx;
static EventTrackerNode_t nodeBuffer[EVENT_TRACKER_CAPACITY] = {0};

static void set_next_free_idx()
{
    if (length == 0)
    {
        nextFreeIdx = 0;
        return;
    }

    int8_t idx = 0;

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
    headIdx = -1;
    tailIdx = -1;
    length = 0;
    nextFreeIdx = 0;

    int8_t idx = 0;

    for (idx = 0; idx < EVENT_TRACKER_CAPACITY; idx++)
    {
        nodeBuffer[idx].thisIdx = idx;
        nodeBuffer[idx].nextIdx = -1;
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

    int8_t targetIdx;

    if (length == 0)
    {
    	length = 1;
        targetIdx = 0;
        headIdx = 0;
    }
    else
    {
		length++;
        targetIdx = nextFreeIdx;
        nodeBuffer[tailIdx].nextIdx = targetIdx;

		if (length < EVENT_TRACKER_CAPACITY)
		{
			set_next_free_idx();
		}
    }

    nodeBuffer[targetIdx].event = newEvent;
    nodeBuffer[targetIdx].nextIdx = -1;
    nodeBuffer[targetIdx].used = true;
    tailIdx = targetIdx;

    return &(nodeBuffer[targetIdx].event);
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

    int8_t dismissed = 0;
    uint8_t jobIdx = 0;
    int8_t currentIdx = headIdx;
    int8_t prevIdx = -1;

    do
    {
        dismissed = 0;

        for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
        {
            if (nodeBuffer[currentIdx].event.jobs[jobIdx].status >= 0)
            {
            	continue;
            }
            else
            {
				if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_FAILED)
				{
					dismissed = 3;
					break;
				}

				if (dismissed < 2 && nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_DISMISSED)
				{
					dismissed = 2;
				}
				else if (dismissed < 1 && nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_HANDLED)
				{
					dismissed = 1;
				}
            }
        }

        if (dismissed == 0)
        {
            prevIdx = currentIdx;
        }
        else
        {
			serial_printer_spool_chars("Event Tracker: freeing event ");
			serial_printer_spool_string((String_t *)&eventTemplates[nodeBuffer[currentIdx].event.eventTemplateIndex].description);
			serial_printer_spool_chars("Cause: ");
			serial_printer_spool_chars(dismissal_reasons[dismissed]);
            // freeing a node
            nodeBuffer[currentIdx].used = false;
            length--;

            // linked list stuff
            // if this is the head, move forward the head
            if (prevIdx < 0)
            {
                headIdx = nodeBuffer[currentIdx].nextIdx;
            }
            // if this is the tail, move back the tail
            if (nodeBuffer[currentIdx].nextIdx < 0)
            {
                tailIdx = prevIdx;
            }
            else if (prevIdx >= 0)
            {
                nodeBuffer[prevIdx].nextIdx = nodeBuffer[currentIdx].nextIdx;
            }
        }

        currentIdx = nodeBuffer[currentIdx].nextIdx;
    } while (currentIdx >= 0);

    taskEXIT_CRITICAL();
}

int8_t event_tracker_get_remaining_storage()
{
    return (EVENT_TRACKER_CAPACITY - length);
}

int8_t event_tracker_get_length()
{
    return length;
}
