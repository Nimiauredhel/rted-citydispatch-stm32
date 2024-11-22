/*
 * date_time.h
 *
 *  Created on: Nov 22, 2024
 *      Author: mickey
 */

#ifndef DATE_TIME_H_
#define DATE_TIME_H_

#include "stm32f7xx_hal.h"
#include "main.h"
#include "serial_output.h"
#include <stdlib.h>

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void date_time_alarm_reset();
void date_time_print();
void date_time_set();

#endif /* DATE_TIME_H_ */
