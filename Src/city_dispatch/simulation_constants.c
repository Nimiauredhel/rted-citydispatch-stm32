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

// Events will be generated, randomly or otherwise,
// from this pool of event templates
const CityEventTemplate_t eventTemplates[8] =
{
    {pdMS_TO_TICKS(2000),  pdMS_TO_TICKS(5000),  MEDICAL, "Minor Medical"},
    {pdMS_TO_TICKS(6000),  pdMS_TO_TICKS(12000), MEDICAL, "Major Medical"},
    {pdMS_TO_TICKS(2000),  pdMS_TO_TICKS(4000),  POLICE,  "Minor Criminal"},
    {pdMS_TO_TICKS(5000),  pdMS_TO_TICKS(10000), POLICE,  "Major Criminal"},
    {pdMS_TO_TICKS(1000),  pdMS_TO_TICKS(4000),  FIRE,    "Minor Fire"},
    {pdMS_TO_TICKS(6000),  pdMS_TO_TICKS(16000), FIRE,    "Major Fire"},
    {pdMS_TO_TICKS(4000),  pdMS_TO_TICKS(6000),  COVID,   "Covid-19 Isolated"},
    {pdMS_TO_TICKS(10000), pdMS_TO_TICKS(10000), COVID,   "Covid-19 Outbreak"},
};

#endif /* SIMULATION_CONSTANTS_C_ */
