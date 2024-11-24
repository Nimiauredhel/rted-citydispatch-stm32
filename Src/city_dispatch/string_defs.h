/*
 * string_defs.h
 *
 *  Created on: Nov 24, 2024
 *      Author: mickey
 */

#ifndef STRING_DEFS_H_
#define STRING_DEFS_H_

#include <stdint.h>

/* Hard-coded structured string structs.
 * felt smart might delete later
 */
typedef struct String
{
	uint8_t size;
	char text[];
} String_t;
typedef struct String6
{
	uint8_t size;
	char text[7];
} String6_t;
typedef struct String14
{
	uint8_t size;
	char text[15];
} String14_t;
typedef struct String22
{
	uint8_t size;
	char text[23];
} String22_t;
typedef struct String30
{
	uint8_t size;
	char text[31];
} String30_t;
typedef struct String38
{
	uint8_t size;
	char text[39];
} String38_t;
typedef struct String46
{
	uint8_t size;
	char text[47];
} String46_t;
typedef struct String54
{
	uint8_t size;
	char text[55];
} String54_t;
typedef struct String62
{
	uint8_t size;
	char text[63];
} String62_t;

#endif /* STRING_DEFS_H_ */
