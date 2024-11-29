/*
 * event_config.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#include "event_config.h"

// Events will be generated, randomly or otherwise,
// from this pool of event templates
const CityEventTemplate_t eventTemplates[NUM_EVENT_TEMPLATES] =
{
    {pdMS_TO_TICKS(2000),  pdMS_TO_TICKS(5000),  MEDICAL, "Minor Medical Incident.\n\r"},
    {pdMS_TO_TICKS(6000),  pdMS_TO_TICKS(12000), MEDICAL, "Major Medical Incident.\n\r"},
    {pdMS_TO_TICKS(2000),  pdMS_TO_TICKS(4000),  POLICE,  "Minor Criminal Incident.\n\r"},
    {pdMS_TO_TICKS(5000),  pdMS_TO_TICKS(10000), POLICE,  "Major Criminal Incident.\n\r"},
    {pdMS_TO_TICKS(1000),  pdMS_TO_TICKS(4000),  FIRE,    "Minor Fire.\n\r"},
    {pdMS_TO_TICKS(6000),  pdMS_TO_TICKS(16000), FIRE,    "Major Fire.\n\r"},
    {pdMS_TO_TICKS(4000),  pdMS_TO_TICKS(6000),  COVID,   "Covid-19 Isolated Incident.\n\r"},
    {pdMS_TO_TICKS(10000), pdMS_TO_TICKS(10000), COVID,   "Covid-19 Outbreak\n\r"},
};

