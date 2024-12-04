/*
 * city_event_tracker.h
 *
 *  Created on: Dec 3, 2024
 *      Author: mickey
 */

#ifndef CITY_DISPATCH_CITY_EVENT_TRACKER_H_
#define CITY_DISPATCH_CITY_EVENT_TRACKER_H_

#include <stdint.h>
#include "simulation_defs.h"
#include "city_events.h"
#include "serial_printer.h"

// the city event tracker is an array backed linked-list
// with a fixed capacity. the logic is such that the tracker
// does not need to have a large capacity since the city
// will be out of resources long before it is filled.
// if the dispatcher finds that the tracker is full while
// trying to add an event, it should either stall until more
// space is freed, or dismiss a lower priority event.

typedef struct EventTrackerNode
{
    bool used;
    uint8_t storageIdx;
    struct EventTrackerNode *next;
    CityEvent_t event;
} EventTrackerNode_t;

void event_tracker_initialize();
CityEvent_t *event_tracker_add(CityEvent_t newEvent);
void event_tracker_refresh();
uint8_t event_tracker_get_remaining_storage();
uint8_t event_tracker_get_length();

#endif /* CITY_DISPATCH_CITY_EVENT_TRACKER_H_ */
