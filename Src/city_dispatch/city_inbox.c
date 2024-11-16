/*
 * city_inbox.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#include "city_inbox.h"

/*
 * Private City Inbox Variables
 */
static uint8_t inboxLowPriorityQueueBuffer[ INCOMING_QUEUE_LENGTH * sizeof( CityEvent_t ) ];
static uint8_t inboxMediumPriorityQueueBuffer[ INCOMING_QUEUE_LENGTH * sizeof( CityEvent_t ) ];
static uint8_t inboxHighPriorityQueueBuffer[ INCOMING_QUEUE_LENGTH * sizeof( CityEvent_t ) ];

static StaticQueue_t inboxLowPriorityQueueControlBlock;
static StaticQueue_t inboxMediumPriorityQueueControlBlock;
static StaticQueue_t inboxHighPriorityQueueControlBlock;

static const osMessageQueueAttr_t inboxLowPriorityQueue_attributes = {
  .name = "inboxLowPriorityQueue",
  .cb_mem = &inboxLowPriorityQueueControlBlock,
  .cb_size = sizeof(inboxLowPriorityQueueControlBlock),
  .mq_mem = &inboxLowPriorityQueueBuffer,
  .mq_size = sizeof(inboxLowPriorityQueueBuffer)
};

const osMessageQueueAttr_t inboxMediumPriorityQueue_attributes = {
  .name = "inboxMediumPriorityQueue",
  .cb_mem = &inboxMediumPriorityQueueControlBlock,
  .cb_size = sizeof(inboxMediumPriorityQueueControlBlock),
  .mq_mem = &inboxMediumPriorityQueueBuffer,
  .mq_size = sizeof(inboxMediumPriorityQueueBuffer)
};

const osMessageQueueAttr_t inboxHighPriorityQueue_attributes = {
  .name = "inboxHighPriorityQueue",
  .cb_mem = &inboxHighPriorityQueueControlBlock,
  .cb_size = sizeof(inboxHighPriorityQueueControlBlock),
  .mq_mem = &inboxHighPriorityQueueBuffer,
  .mq_size = sizeof(inboxHighPriorityQueueBuffer)
};

/*
 * Public City Inbox Instance
 */
CityInbox_t city_inbox;


/*
 * Public City Inbox Initialization Function
 */
void city_inbox_initialize()
{
  city_inbox.inboxLowPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxLowPriorityQueue_attributes);
  city_inbox.inboxMediumPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxMediumPriorityQueue_attributes);
  city_inbox.inboxHighPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxHighPriorityQueue_attributes);
}
