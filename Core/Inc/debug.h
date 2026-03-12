/*
 * debug.h
 *
 *  Created on: 12 Mar 2026
 *      Author: Luproger
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "stm32f1xx_hal.h"
#include <stdio.h>

#define DEBUG_MODE

#ifdef DEBUG_MODE
	#define DEBUG_INIT(huart) debug_init(huart)
	#define DEBUG_PRINTF(format, ...) myprintf(format, ##__VA_ARGS__)
#else
	#define DEBUG_INIT(huart)
	#define DEBUG_PRINTF(format, ...)
#endif

void debug_init(UART_HandleTypeDef *huart);

void myprintf(char *format, ...);

#endif /* INC_DEBUG_H_ */
