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

#endif /* SIMULATION_CONSTANTS_C_ */
