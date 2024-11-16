/*
 * utils.h
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <string.h>
#include "simulation_defs.h"

/* Functions to generate the hard-coded structured string structs.
 * felt smart might delete later
 */
String6_t utils_structure_string6(char *string);
String14_t utils_structure_string14(char *string);
String22_t utils_structure_string22(char *string);
String30_t utils_structure_string30(char *string);
String38_t utils_structure_string38(char *string);
String46_t utils_structure_string46(char *string);
String54_t utils_structure_string54(char *string);
String62_t utils_structure_string62(char *string);
String_t *utils_structure_string_alloc(char *string);

#endif /* UTILS_H_ */
