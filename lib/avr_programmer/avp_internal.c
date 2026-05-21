/*
 * avp_internal.c
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */
#include "avp_internal.h"

avp_spi_conf *spiConf;
avp_uart_conf *uartConf;

avp_action curAction;
const avp_init_t *avr_prog;
avp_param_t *param;

bool AVP_ERROR = 0;
char errMessage[64];
const char* errType;

FIL firmwareFile = {0};
avp_ftype firmwareFtype;

uint8_t flash_buf[MAX_PAGE_SIZE_BYTES];
uint32_t f_page_size_b;

void FAIL(const char* error_type, const char* fmt, ...){
	errType = error_type;
	va_list args;
	va_start(args, fmt);
	vsnprintf(errMessage, sizeof(errMessage), fmt, args);
	va_end(args);

	AVP_ERROR = 1; // ОЩИБКААААА
}

void null_actFunc(){
	FAIL(AVP_ERR_INIT, AVP_ERR_ACTION);
}
