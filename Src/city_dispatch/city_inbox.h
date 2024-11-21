/*
 * city_inbox.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef CITY_INBOX_H_
#define CITY_INBOX_H_

#include "cmsis_os.h"
#include "simulation_defs.h"

typedef struct CityInbox
{
	osMessageQueueId_t inboxLowPriorityQueueHandle;
	osMessageQueueId_t inboxMediumPriorityQueueHandle;
	osMessageQueueId_t inboxHighPriorityQueueHandle;
} CityInbox_t;

extern CityInbox_t city_inbox;

void city_inbox_initialize();
void city_inbox_clear();

#endif /* CITY_INBOX_H_ */
