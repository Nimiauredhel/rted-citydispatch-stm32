/*
 * event_config.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#include "city_events.h"

// the different job types that can be associated
// with different events, and must be each handled
// for an event to be considered handled as a whole.
// index 0 holds the "null job"
const CityJobTemplate_t jobTemplates[NUM_JOB_TEMPLATES] =
{
    { DEPT_EMPTY, 0, 0, { 12, "NULL JOB.\n\r"} },
    { MEDICAL, 2,  5,   { 18, "Ambulance Ride.\n\r" } },
    { MEDICAL, 4,  8,   { 22, "Medical Evacuation.\n\r" } },
    { MEDICAL, 6,  12,  { 19, "Field Treatment.\n\r"} },
    { POLICE,  2,  4,   { 18, "Suspect Arrest.\n\r"} },
    { POLICE,  2,  4,   { 29, "Low Stakes Enforcement.\n\r"} },
    { POLICE,  5,  10,  { 28, "High Stakes Enforcement.\n\r"} },
    { FIRE,    1,  4,   { 26, "Minor Fire Suppression.\n\r"} },
    { FIRE,    4,  8,   { 10, "Rescue.\n\r"} },
    { FIRE,    6,  16,  { 26, "Major Fire Suppression.\n\r"} },
    { COVID,   7,  14,  { 23, "Covid-19 Quarantine.\n\r"} },
    { COVID,   14, 28,  { 22, "Covid-19 Treatment.\n\r"} },
};
// Events will be generated, randomly or otherwise,
// from this pool of event templates
const CityEventTemplate_t eventTemplates[NUM_EVENT_TEMPLATES] =
{
    { EVENT_NORMAL, 20,  50,  { 17, "Work Accident.\n\r" },    {2, 3, 0, 0} },
    { EVENT_HIGH,   60,  120, { 17, "Violent Crime.\n\r" },    {3, 4, 6, 0} },
    { EVENT_LOW,    20,  40,  { 27, "Noise Complaint.\n\r" },  {5, 0, 0, 0} },
    { EVENT_HIGH,   50,  100, { 27, "Vehicle Crash.\n\r" },    {2, 3, 3, 5} },
    { EVENT_HIGH,   10,  40,  { 19, "Serial Arsonist.\n\r" },  {4, 7, 7, 9} },
    { EVENT_HIGH,   60,  100, { 16, "Factory Fire.\n\r" },     {2, 3, 8, 9} },
    { EVENT_NORMAL, 80,  100, { 17, "Covid-19 Case.\n\r" },    {2, 3, 0, 0} },
    { EVENT_HIGH,   160, 200, { 20, "Covid-19 Outbreak\n\r" }, {2, 3, 0, 0} },
};

