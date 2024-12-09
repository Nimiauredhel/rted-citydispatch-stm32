/*
 * event_config.c
 *
 *  Created on: Nov 5, 2024
 *      Author: mickey
 */

#include "city_events.h"

/*
 * The different job types that can be associated
 * with different events, and must each be handled
 * for an event to be considered handled as a whole.
 * index 0 holds the "null job", required since
 * the event job arrays are fixed-size.
 */
const CityJobTemplate_t jobTemplates[NUM_JOB_TEMPLATES] =
{
    { DEPT_EMPTY, 0, 0, "NULL JOB" },
    { MEDICAL, 2,  5,   "Ambulance Ride" },
    { MEDICAL, 4,  8,   "Medical Evacuation" },
    { MEDICAL, 6,  12,  "Field Treatment" },
    { POLICE,  2,  4,   "Suspect Arrest" },
    { POLICE,  2,  4,   "Low Stakes Enforcement" },
    { POLICE,  5,  10,  "High Stakes Enforcement" },
    { FIRE,    1,  4,   "Minor Fire Suppression" },
    { FIRE,    4,  8,   "Rescue" },
    { FIRE,    6,  16,  "Major Fire Suppression" },
    { COVID,   7,  14,  "Covid-19 Quarantine" },
    { COVID,   14, 28,  "Covid-19 Treatment" },
};
/*
 * Events will be generated, randomly or otherwise,
 * from this pool of event templates.
 * Each event is comprised of: a priority, minimum and maximum expiration times,
 * a description string, and a fixed-size array of associated jobs.
 * An event instance is considered handled when all associated job instances
 * have been handled by city agents. It is considered failed when its expiration time
 * has passed before being handled.
 * TODO: implement the fail state. Currently events cannot fail.
 */
const CityEventTemplate_t eventTemplates[NUM_EVENT_TEMPLATES] =
{
    { EVENT_LOW,    600, 1200, "Stubbed Toe",       {1, 0, 0, 0, 0, 0, 0, 0} },
    { EVENT_NORMAL, 20,  50,   "Work Accident",     {2, 3, 0, 0, 0, 0, 0, 0} },
    { EVENT_HIGH,   60,  120,  "Violent Crime",     {3, 3, 4, 4, 6, 0, 0, 0} },
    { EVENT_LOW,    20,  40,   "Noise Complaint",   {5, 0, 0, 0, 0, 0, 0, 0} },
    { EVENT_HIGH,   50,  100,  "Vehicle Crash",     {2, 2, 3, 3, 5, 0, 0, 0} },
    { EVENT_HIGH,   10,  40,   "Serial Arsonist",   {4, 4, 7, 7, 9, 0, 0, 0} },
    { EVENT_HIGH,   60,  100,  "Factory Fire",      {2, 3, 3, 3, 8, 8, 9, 9} },
    { EVENT_NORMAL, 80,  100,  "Covid-19 Case",     {2, 10, 11, 0, 0, 0, 0, 0} },
    { EVENT_HIGH,   80,  100,  "Covid-19 Outbreak", {2, 2, 2, 10, 10, 10, 11, 11} },
};

