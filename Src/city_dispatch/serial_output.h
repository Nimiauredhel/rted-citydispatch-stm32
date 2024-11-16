/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SERIAL_OUTPUT_H_
#define SERIAL_OUTPUT_H_

#include <stdio.h>
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

void serial_printer_initialize(void);
void serial_printer_spool_chars(char *string);
void serial_printer_spool_string(String_t *string);

#endif /* SERIAL_OUTPUT_H_ */
