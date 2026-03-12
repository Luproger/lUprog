/*
 * debug.c
 *
 *  Created on: 12 Mar 2026
 *      Author: Luproger
 */
#include "debug.h"
#include <stdarg.h>

UART_HandleTypeDef *debugHuart;
uint8_t printBufer[128];

void debug_init(UART_HandleTypeDef *huart){
	debugHuart = huart;
}

void myprintf(char *format, ...) {
	if (debugHuart == NULL) return;
	va_list args;
	va_start(args, format);
	int len = vsnprintf((char*) printBufer, sizeof(printBufer), format, args);
	va_end(args);
	HAL_UART_Transmit(debugHuart, printBufer, len, 1000);
}

