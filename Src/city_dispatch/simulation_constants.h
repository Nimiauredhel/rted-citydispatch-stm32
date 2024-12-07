/*
 * simulation_constants.h
 *
 *  Created on: Nov 12, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_CONSTANTS_H_
#define SIMULATION_CONSTANTS_H_

#include "simulation_defs.h"

extern const char departmentNames[NUM_DEPARTMENTS][10];
extern const uint8_t departmentAgentCounts[NUM_DEPARTMENTS];

extern const TickType_t DELAY_10MS_TICKS;
extern const TickType_t DELAY_100MS_TICKS;
extern const TickType_t DELAY_300MS_TICKS;
extern const TickType_t DELAY_500MS_TICKS;
extern const TickType_t DELAY_1000MS_TICKS;

#endif /* SIMULATION_CONSTANTS_H_ */
