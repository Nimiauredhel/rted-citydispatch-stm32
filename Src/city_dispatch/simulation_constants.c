/*
 * simulation_constants.c
 *
 *  Created on: Nov 12, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_CONSTANTS_C_
#define SIMULATION_CONSTANTS_C_

#include "simulation_constants.h"

const char departmentNames[NUM_DEPARTMENTS][10] = {"Medical\0", "Police\0", "Fire\0", "Covid-19\0"};
const uint8_t departmentAgentCounts[NUM_DEPARTMENTS] = {4, 3, 2, 4};

const TickType_t DELAY_10MS_TICKS = pdMS_TO_TICKS(10);
const TickType_t DELAY_100MS_TICKS = pdMS_TO_TICKS(100);
const TickType_t DELAY_300MS_TICKS = pdMS_TO_TICKS(300);
const TickType_t DELAY_500MS_TICKS = pdMS_TO_TICKS(500);
const TickType_t DELAY_1000MS_TICKS = pdMS_TO_TICKS(1000);

#endif /* SIMULATION_CONSTANTS_C_ */
