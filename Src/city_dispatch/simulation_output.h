/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_OUTPUT_H_
#define SIMULATION_OUTPUT_H_

#include <string.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

void output_print_blocking(char *string, uint16_t size);
void output_print_blocking_autosize(char *string);
void output_print_irq(char *string, uint16_t size);
void output_print_irq_autosize(char *string);

#endif /* SIMULATION_OUTPUT_H_ */
