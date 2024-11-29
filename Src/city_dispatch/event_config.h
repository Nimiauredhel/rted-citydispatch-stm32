/*
 * event_config.h
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#ifndef EVENT_CONFIG_H_
#define EVENT_CONFIG_H_

#define NUM_EVENT_TEMPLATES 8

#include "simulation_defs.h"

typedef struct CityEventTemplate
{
    TickType_t minTicks;
    TickType_t maxTicks;
    DepartmentCode_t code;
    char *description;
} CityEventTemplate_t;

// Events will be generated, randomly or otherwise,
// from this pool of event templates
extern const CityEventTemplate_t eventTemplates[NUM_EVENT_TEMPLATES];

#endif /* EVENT_CONFIG_H_ */
