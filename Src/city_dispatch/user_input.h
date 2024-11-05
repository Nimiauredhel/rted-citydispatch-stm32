/*
 * user_input.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include "simulation_control.h"
#include "simulation_output.h"
#include "FreeRTOS.h"

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

#endif /* USER_INPUT_H_ */
