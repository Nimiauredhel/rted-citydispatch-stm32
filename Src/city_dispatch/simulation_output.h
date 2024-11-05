/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SIMULATION_OUTPUT_H_
#define SIMULATION_OUTPUT_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

void output_print_blocking(char *string, size_t length);
void output_print_irq(char *string, size_t length);

#endif /* SIMULATION_OUTPUT_H_ */
