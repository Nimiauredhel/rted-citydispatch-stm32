/*
 * serial_output.h
 *
 *  Created on: Nov 21, 2024
 *      Author: mickey
 */

#ifndef SERIAL_OUTPUT_H_
#define SERIAL_OUTPUT_H_

#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "string_defs.h"

void output_print_blocking_autosize(const char *string);
void output_print_irq(const char *string, uint16_t size);
void output_print_string_irq(const String_t *string);
void output_print_irq_autosize(const char *string);
void output_print_blocking(const char *string, uint16_t size);
void output_print_string_blocking(const String_t *string);

#endif /* SERIAL_OUTPUT_H_ */
