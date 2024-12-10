/*
 *
 *  Created on: Dec 3, 2024
 *      Author: mickey
 */

/** @addtogroup City_Dispatch
  * @{
  */

/**
 * @file city_event_tracker.c
 * @brief Implements the "Event Tracker" structure, where pending events are stored,
 * until they reach a final outcome state (handled, failed, or otherwise dismissed).
 * It is necessary since a single event contains a multitude of associated jobs
 * that often require multiple agents from different departments to handle,
 * which means that an event cannot be simply passed on to a singular department or agent.
 */

#include "city_event_tracker.h"

/// The different outcome states that events may reach.
/// TODO: Implement the fail states. Currently events may not fail.
typedef enum EventOutcome
{
	OUTCOME_PENDING = 0,
	OUTCOME_SUCCESS = 1,
	OUTCOME_FAILURE = 2,
	OUTCOME_CANCELLED = 3,
} EventOutcome_t;

osMutexId_t eventTrackerMutexHandle;
StaticSemaphore_t eventTrackerMutexControlBlock;
const osMutexAttr_t eventTrackerMutex_attributes = {
  .name = "eventTrackerMutex",
  .cb_mem = &eventTrackerMutexControlBlock,
  .cb_size = sizeof(eventTrackerMutexControlBlock),
};

/// The is_dirty flag is set whenever an ongoing job state is altered,
/// and cleared when the event tracker is "refreshed" (e.g. when jobs are cleaned up).
/// It allows the event tracker refresh function to only run as necessary.
static bool is_dirty = false;
/// The event tracker holds "linked list" style variables since it uses
/// linked list style operation to iterate on its active job objects.
/// This allows it to only iterate on array cells that are in use.
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
    eventTrackerMutexHandle = osMutexNew(&eventTrackerMutex_attributes);
    osMutexAcquire(eventTrackerMutexHandle, osWaitForever);

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
    serial_printer_spool_log(log_buffer);

    osMutexRelease(eventTrackerMutexHandle);
}

// if successful, returns pointer to stored address
// if failed, returns NULL
CityEvent_t *event_tracker_add(CityEvent_t newEvent)
{
    osMutexAcquire(eventTrackerMutexHandle, osWaitForever);

    log_buffer.subject_0 = LOGSBJ_EVENT;
    log_buffer.subject_1 = newEvent.eventTemplateIndex;

    if (nextFreeIdx < 0 || length >= EVENT_TRACKER_CAPACITY)
    {
		log_buffer.format = LOGFMT_DISMISSING;
		log_buffer.subject_2 = LOGSBJ_INSUFFICIENT_SPACE;
		osMutexRelease(eventTrackerMutexHandle);
		serial_printer_spool_log(log_buffer);
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
    osMutexRelease(eventTrackerMutexHandle);

    log_buffer.subject_2 = targetIdx;
    log_buffer.format = LOGFMT_REGISTERED;
	serial_printer_spool_log(log_buffer);

    return &(nodeBuffer[targetIdx].event);
}

void event_tracker_refresh()
{
    if (length == 0) return;
    if (headIdx == -1 || tailIdx == -1) return;

    log_buffer.format = LOGFMT_REFRESHING;
    serial_printer_spool_log(log_buffer);

    osMutexAcquire(eventTrackerMutexHandle, osWaitForever);

    uint8_t jobIdx = 0;
    int8_t currentIdx = headIdx;
    int8_t prevIdx = -1;
    int8_t temp = -1;

    do
    {
		osDelay(DELAY_10MS_TICKS);
		uint8_t pendingCount = 0;
		uint8_t failedCount = 0;
		uint8_t cancelledCount = 0;
		uint8_t handledCount = 0;
		bool canRemove = false;
		EventOutcome_t outcome = OUTCOME_PENDING;

		// loop through event jobs to determine if the event can be dismissed
        for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
        {
            if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_NONE
            	|| nodeBuffer[currentIdx].event.jobs[jobIdx].code == DEPT_EMPTY)
            {
            	continue;
            }
            else
            {
            	bool acquiredMutex = false;
            	// if this job doesn't have an assigned agent mutex yet,
            	// it has likely been released or not yet assigned,
            	// so it will be counted, but not altered
            	if (nodeBuffer[currentIdx].event.jobs[jobIdx].assignedAgentMutex != NULL)
            	{
					osMutexAcquire(nodeBuffer[currentIdx].event.jobs[jobIdx].assignedAgentMutex, osWaitForever);
					acquiredMutex = true;
            	}

				switch (nodeBuffer[currentIdx].event.jobs[jobIdx].status)
				{
					case JOB_PENDING:
					case JOB_ONGOING:
						pendingCount++;
						break;
					case JOB_FAILED_TRACKER:
						failedCount++;
						pendingCount++;
						break;
					case JOB_FAILED_AGENT:
						failedCount++;
						break;
					case JOB_CANCELLED_AGENT:
						cancelledCount++;
						break;
					case JOB_CANCELLED_TRACKER:
						cancelledCount++;
						pendingCount++;
						break;
					case JOB_HANDLED_AGENT:
						handledCount++;
						break;
					default:
						break;
				}

				if (acquiredMutex)
				{
					osMutexRelease(nodeBuffer[currentIdx].event.jobs[jobIdx].assignedAgentMutex);
				}
            }
        }

		// use results of job loop to determine the event outcome
		// in case of failure or deprioritization,
		// one failed/dismissed job is enough
		// to invalidate the whole event,
		// but one pending job is enough to delay its release.
		if (pendingCount == 0)
		{
			canRemove = true;
			outcome = failedCount > 0 ? OUTCOME_FAILURE
					: cancelledCount > 0 ? OUTCOME_CANCELLED
					: OUTCOME_SUCCESS;
		}
		else if (failedCount > 0 || cancelledCount > 0)
		{
			for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
			{
				if (nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_PENDING
					|| nodeBuffer[currentIdx].event.jobs[jobIdx].status == JOB_ONGOING)
				{
					nodeBuffer[currentIdx].event.jobs[jobIdx].status =
					failedCount > 0 ? JOB_FAILED_TRACKER : JOB_CANCELLED_TRACKER;
				}
			}
		}

        if (canRemove)
        {
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

			log_buffer.format = LOGFMT_REMOVING;
			log_buffer.subject_0 = LOGSBJ_EVENT;
			log_buffer.subject_1 = nodeBuffer[currentIdx].event.eventTemplateIndex;
			log_buffer.subject_2 =
					outcome == OUTCOME_SUCCESS ? LOGSBJ_SUCCESS
					: outcome == OUTCOME_FAILURE ? LOGSBJ_FAILURE
					: LOGSBJ_DEPRIORITIZED;
			serial_printer_spool_log(log_buffer);
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
    osMutexRelease(eventTrackerMutexHandle);
}

void event_tracker_set_dirty()
{
    osMutexAcquire(eventTrackerMutexHandle, osWaitForever);
	is_dirty = true;
    osMutexRelease(eventTrackerMutexHandle);
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

/** 
  * @}
  */
