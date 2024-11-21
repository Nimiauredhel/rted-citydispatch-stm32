/*
 * simulation_output.h
 *
 *  Created on: Nov 2, 2024
 *      Author: mickey
 */

#ifndef SERIAL_PRINTER_H_
#define SERIAL_PRINTER_H_

#include "serial_output.h"

void serial_printer_initialize(void);
void serial_printer_spool_chars(char *string);
void serial_printer_spool_string(String_t *string);

#endif /* SERIAL_PRINTER_H_ */
