/*
 * utils.c
 *
 *  Created on: Nov 16, 2024
 *      Author: mickey
 */

#include "utils.h"

String6_t utils_structure_string6(char *string)
{
	uint8_t size = strlen(string) + 1;
	String6_t new_string;

	if (size > 7)
	{
		new_string.size = 7;
		strcpy(new_string.text, "[St>7]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String14_t utils_structure_string14(char *string)
{
	uint8_t size = strlen(string) + 1;
	String14_t new_string;

	if (size > 15)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>15]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String22_t utils_structure_string22(char *string)
{
	uint8_t size = strlen(string) + 1;
	String22_t new_string;

	if (size > 23)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>22]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String30_t utils_structure_string30(char *string)
{
	uint8_t size = strlen(string) + 1;
	String30_t new_string;

	if (size > 31)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>30]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String38_t utils_structure_string38(char *string)
{
	uint8_t size = strlen(string) + 1;
	String38_t new_string;

	if (size > 39)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>38]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String46_t utils_structure_string46(char *string)
{
	uint8_t size = strlen(string) + 1;
	String46_t new_string;

	if (size > 47)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>46]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String54_t utils_structure_string54(char *string)
{
	uint8_t size = strlen(string) + 1;
	String54_t new_string;

	if (size > 55)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>54]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}
String62_t utils_structure_string62(char *string)
{
	uint8_t size = strlen(string) + 1;
	String62_t new_string;

	if (size > 63)
	{
		new_string.size = 8;
		strcpy(new_string.text, "[St>62]");
	}
	else
	{
		new_string.size = size;
		strcpy(new_string.text, string);
	}

	return new_string;
}

String_t *utils_structure_string_alloc(char *string)
{
	uint8_t length = strlen(string);
	void *ptr;

	switch (length) {
		case 0 ... 6:
			ptr = malloc(sizeof(String6_t));
			*(String6_t *)ptr = utils_structure_string6(string);
			break;
		case 7 ... 14:
			ptr = malloc(sizeof(String14_t));
			*(String14_t *)ptr = utils_structure_string14(string);
			break;
		case 15 ... 22:
			ptr = malloc(sizeof(String22_t));
			*(String22_t *)ptr = utils_structure_string22(string);
			break;
		case 23 ... 30:
			ptr = malloc(sizeof(String30_t));
			*(String30_t *)ptr = utils_structure_string30(string);
			break;
		case 31 ... 38:
			ptr = malloc(sizeof(String38_t));
			*(String38_t *)ptr = utils_structure_string38(string);
			break;
		case 39 ... 46:
			ptr = malloc(sizeof(String46_t));
			*(String46_t *)ptr = utils_structure_string46(string);
			break;
		case 47 ... 54:
			ptr = malloc(sizeof(String54_t));
			*(String54_t *)ptr = utils_structure_string54(string);
			break;
		case 55 ... 62:
			ptr = malloc(sizeof(String62_t));
			*(String62_t *)ptr = utils_structure_string62(string);
			break;
		default:
			return NULL;
	}

	return (String_t *)ptr;
}
