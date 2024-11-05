/*
 * simulation_output.c
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#include "simulation_output.h"

extern UART_HandleTypeDef huart3;

void output_print_blocking(char *string, size_t length)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)string, length, 0xFFFF);
}

void output_print_irq(char *string, size_t length)
{
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)string, length);
}
