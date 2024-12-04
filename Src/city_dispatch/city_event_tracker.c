/*
 *
 *  Created on: Dec 3, 2024
 *      Author: mickey
 */

#include "city_event_tracker.h"

uint8_t length = 0;
uint8_t nextFreeIdx = 0;
EventTrackerNode_t *head;
EventTrackerNode_t *tail;
EventTrackerNode_t nodeBuffer[EVENT_TRACKER_CAPACITY] = {0};

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
}

// if successful, returns remaining space
// if failed, returns -1
int16_t event_tracker_add(CityEvent_t newEvent)
{
    if (length >= EVENT_TRACKER_CAPACITY)
    {
        return -1;
    }

    EventTrackerNode_t targetNode;

    if (length == 0)
    {
        targetNode = nodeBuffer[0];
        head = &targetNode;
    }
    else
    {
        targetNode = nodeBuffer[nextFreeIdx];
        tail->next = &targetNode;
    }

    targetNode.event = newEvent;
    targetNode.next = NULL;
    tail = &targetNode;

    length++;

    if (length < EVENT_TRACKER_CAPACITY)
    {
        set_next_free_idx();
    }

    return event_tracker_get_remaining_storage();
}

void event_tracker_refresh()
{
    if (length == 0) return;

    bool active = false;
    uint8_t jobIdx = 0;
    EventTrackerNode_t *current = head;
    EventTrackerNode_t *prev = NULL;

    do
    {
        active = false;

        for (jobIdx = 0; jobIdx < NUM_EVENT_JOBS; jobIdx++)
        {
            if (current->event.jobs[jobIdx].status > 0)
            {
                active = true;
                break;
            }
        }

        if (active)
        {
            prev = current;
        }
        else
        {
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
}

uint8_t event_tracker_get_remaining_storage()
{
    return (EVENT_TRACKER_CAPACITY - length);
}
