/*
 * city_inbox.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

/*
 * The "City Inbox" is the collection of prioritized input queues
 * from which the "City Dispatcher" task accepts new events as input.
 * It is only written to from the "Event Generator" task.
 * This module is separate from the dispatcher module itself to reduce
 * coupling between the current event generator and other parts of the simulation,
 * as it may later be desirable to replace it with a different input source.
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

static CityLog_t log_buffer;

/*
 * Public City Inbox Instance
 */
CityInbox_t city_inbox;

/*
 * @brief The city inbox initialization function,
 * which creates the prioritized message queues.
 * @retval None
 */
void city_inbox_initialize()
{
	city_inbox.inboxLowPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxLowPriorityQueue_attributes);
	city_inbox.inboxMediumPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxMediumPriorityQueue_attributes);
	city_inbox.inboxHighPriorityQueueHandle = osMessageQueueNew (INCOMING_QUEUE_LENGTH, sizeof(CityEvent_t), &inboxHighPriorityQueue_attributes);

	log_buffer.identifier_0 = LOGID_CITY_INBOX;
	log_buffer.format = LOGFMT_INITIALIZED;
	serial_printer_spool_log(log_buffer);
}

/*
 * @brief The city inbox clear function,
 * which resets the prioritized message queues.
 * @retval None
 */
void city_inbox_clear()
{
	osMessageQueueReset(city_inbox.inboxLowPriorityQueueHandle);
	osMessageQueueReset(city_inbox.inboxMediumPriorityQueueHandle);
	osMessageQueueReset(city_inbox.inboxHighPriorityQueueHandle);

	log_buffer.format = LOGFMT_CLEARED;
	serial_printer_spool_log(log_buffer);
}
