/*
 *
 *  Created on: Dec 3, 2024
 *      Author: mickey
 */

#include "city_event_tracker.h"

#define DISMISSAL_PENDING 0
#define DISMISSAL_SUCCESS 1
#define DISMISSAL_DEPRIORITIZED 2
#define DISMISSAL_FAILURE 3

static bool is_dirty = false;
static int8_t length = 0;
static int8_t nextFreeIdx = 0;
static int8_t headIdx;
static int8_t tailIdx;
static CityLog_t log_buffer;
static EventTrackerNode_t nodeBuffer[EVENT_TRACKER_CAPACITY] = {0};

static void set_next_free_idx()
{
    if (length == 0)
    {
        nextFreeIdx = 0;
        return;
    }

    nextFreeIdx = -1;

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
    log_buffer.identifier_0 = LOGID_EVENT_TRACKER;

    int8_t idx = 0;

    for (idx = 0; idx < EVENT_TRACKER_CAPACITY; idx++)
    {
        nodeBuffer[idx].thisIdx = idx;
        nodeBuffer[idx].nextIdx = -1;
        nodeBuffer[idx].used = false;
    }

    log_buffer.format = LOGFMT_INITIALIZED;
    serial_printer_spool_log(&log_buffer);
}

// if successful, returns pointer to stored address
// if failed, returns NULL
CityEvent_t *event_tracker_add(CityEvent_t newEvent)
{
    log_buffer.subject_0 = LOGSBJ_EVENT;
    log_buffer.subject_1 = newEvent.eventTemplateIndex;

    if (nextFreeIdx < 0 || length >= EVENT_TRACKER_CAPACITY)
    {
		log_buffer.format = LOGFMT_DISMISSING;
		log_buffer.subject_2 = LOGSBJ_INSUFFICIENT_SPACE;
		serial_printer_spool_log(&log_buffer);
        return NULL;
    }

    int8_t targetIdx;

    if (length == 0)
    {
        targetIdx = 0;
        headIdx = 0;
    }
    else
    {
        targetIdx = nextFreeIdx;
        nodeBuffer[tailIdx].nextIdx = targetIdx;
    }

	length++;

    nodeBuffer[targetIdx].event = newEvent;
    nodeBuffer[targetIdx].nextIdx = -1;
    nodeBuffer[targetIdx].used = true;
    tailIdx = targetIdx;

	set_next_free_idx();

    log_buffer.subject_2 = targetIdx;
    log_buffer.format = LOGFMT_REGISTERED;
	serial_printer_spool_log(&log_buffer);

    return &(nodeBuffer[targetIdx].event);
}

void event_tracker_refresh()
{
    if (length == 0) return;
    if (headIdx == -1 || tailIdx == -1) return;

    log_buffer.format = LOGFMT_REFRESHING;
    serial_printer_spool_log(&log_buffer);

    // this function invokes CRITICAL mode
    // due to "job status" being used as IPC
    // between dispatcher & agents.
    // TODO: use mutex instead (to only lock individual resources when required, rather than whole system every time)

    int8_t dismissed = DISMISSAL_PENDING;
    uint8_t jobIdx = 0;
    int8_t currentIdx = headIdx;
    int8_t prevIdx = -1;
    int8_t temp = -1;

    do
    {
		dismissed = DISMISSAL_SUCCESS;

        // determine if event should be dismissed
        for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
        {
            if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_NONE
            	|| nodeBuffer[currentIdx].event.jobs[jobIdx].code == DEPT_EMPTY)
            {
            	continue;
            }
            else
            {
            	// in case of failure or deprioritization,
            	// one failed/dismissed job is enough
            	// to invalidate the whole event.
				if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_FAILED)
				{
					dismissed = DISMISSAL_FAILURE;
					break;
				}

				if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_DISMISSED)
				{
					dismissed = DISMISSAL_DEPRIORITIZED;
					break;
				}

				// in case of success, all job of the event must have been handled
				// for the event to count as successfully cleared.
				if (nodeBuffer[currentIdx].event.jobs[jobIdx].status != JOB_HANDLED)
				{
					dismissed = DISMISSAL_PENDING;
				}
            }
        }

        // dismiss if required
        if (dismissed > 0)
        {
			taskENTER_CRITICAL();

			log_buffer.format = LOGFMT_REMOVING;
			log_buffer.subject_0 = LOGSBJ_EVENT;
			log_buffer.subject_1 = nodeBuffer[currentIdx].event.eventTemplateIndex;
			log_buffer.subject_2 =
					dismissed == DISMISSAL_SUCCESS ? LOGSBJ_COMPLETE
					: dismissed == DISMISSAL_FAILURE ? LOGSBJ_OVERDUE
					: LOGSBJ_DEPRIORITIZED;

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

            temp = currentIdx;
            currentIdx = nodeBuffer[currentIdx].nextIdx;
            nodeBuffer[temp].nextIdx = -1;


			taskEXIT_CRITICAL();
			serial_printer_spool_log(&log_buffer);
        }
        else
        {
            prevIdx = currentIdx;
            currentIdx = nodeBuffer[currentIdx].nextIdx;
        }

    } while (currentIdx >= 0);

    if (nextFreeIdx < 0)
    {
		set_next_free_idx();
    }

	is_dirty = false;
}

void event_tracker_set_dirty()
{
	is_dirty = true;
}

bool event_tracker_get_dirty()
{
	return is_dirty;
}

int8_t event_tracker_get_remaining_storage()
{
    return (EVENT_TRACKER_CAPACITY - length);
}

int8_t event_tracker_get_length()
{
    return length;
}
