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

// Events will be generated, randomly or otherwise,
// from this pool of event templates
extern const CityEventTemplate_t eventTemplates[8];

#endif /* SIMULATION_CONSTANTS_H_ */
