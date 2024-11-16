/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SERIAL_OUTPUT_H_
#define SERIAL_OUTPUT_H_

#include <string.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "simulation_defs.h"

void output_print_blocking(char *string, uint16_t size);
void output_print_string_blocking(String_t *string);
void output_print_blocking_autosize(char *string);
void output_print_irq(char *string, uint16_t size);
void output_print_string_irq(String_t *string);
void output_print_irq_autosize(char *string);

typedef struct OutputState
{
	osStatus_t queue_read_status;
	char output_buffer[48];
} OutputState_t;


void serial_output_initialize(void);

#endif /* SERIAL_OUTPUT_H_ */
