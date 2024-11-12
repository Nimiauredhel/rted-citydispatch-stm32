/*
 * city_inbox.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#include "city_inbox.h"

uint8_t inboxLowPriorityQueueBuffer[ 16 * sizeof( uint16_t ) ];
uint8_t inboxMediumPriorityQueueBuffer[ 16 * sizeof( uint16_t ) ];
uint8_t inboxHighPriorityQueueBuffer[ 16 * sizeof( uint16_t ) ];

StaticQueue_t inboxLowPriorityQueueControlBlock;
StaticQueue_t inboxMediumPriorityQueueControlBlock;
StaticQueue_t inboxHighPriorityQueueControlBlock;

const osMessageQueueAttr_t inboxLowPriorityQueue_attributes = {
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
