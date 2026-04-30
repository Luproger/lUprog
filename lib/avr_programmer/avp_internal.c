/*
 * avp_internal.c
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */
#include "avp_internal.h"

const uint8_t file_supp[ACTION_MAX] = {
	// FLASH
	[ACT_FL_WRITE] = AVP_FTYPE_BIN | AVP_FTYPE_HEX,
	[ACT_FL_READ]  = AVP_FTYPE_BIN | AVP_FTYPE_HEX,
	[ACT_FL_VERIFY] = AVP_FTYPE_BIN | AVP_FTYPE_HEX,

	// EEPROM
	[ACT_EE_WRITE] = AVP_FTYPE_BIN | AVP_FTYPE_HEX,
	[ACT_EE_READ]  = AVP_FTYPE_BIN | AVP_FTYPE_HEX,
	[ACT_EE_VERIFY] = AVP_FTYPE_BIN | AVP_FTYPE_HEX,

	// FUSEBIT
	[ACT_FB_WRITE] = AVP_FTYPE_TXT | AVP_FTYPE_BIN,
	[ACT_FB_DEFAULT] = AVP_FTYPE_DEF,
	[ACT_FB_READ]  = AVP_FTYPE_TXT | AVP_FTYPE_BIN,
	[ACT_FB_VERIFY] = AVP_FTYPE_TXT | AVP_FTYPE_BIN,

	// LOCKBIT
	[ACT_LB_WRITE] = AVP_FTYPE_TXT | AVP_FTYPE_BIN,
	[ACT_LB_READ]  = AVP_FTYPE_TXT | AVP_FTYPE_BIN,
	[ACT_LB_VERIFY] = AVP_FTYPE_TXT | AVP_FTYPE_BIN,

	// CONFIG
	[ACT_CFG_WRITE] = AVP_FTYPE_CFG,
	[ACT_CFG_READ] = AVP_FTYPE_CFG,
	[ACT_CFG_VERIFY] = AVP_FTYPE_CFG
};


avp_spi_conf *spiConf;
avp_uart_conf *uartConf;

const avp_init_t *avr_prog;
avp_param_t *param;

bool AVP_ERROR = 0;
char errMessage[100];

FIL firmwareFile = {0};
avp_ftype firmwareFtype;

uint8_t flash_buf[MAX_PAGE_SIZE_BYTES];
uint32_t f_page_size_b;

void FAIL(const char* fmt, ...){
	va_list args;
	va_start(args, fmt);
	vsnprintf(errMessage, sizeof(errMessage), fmt, args);
	va_end(args);

	AVP_ERROR = 1; // ОЩИБКААААА
}

void null_actFunc(){
	FAIL(AVP_ERR_ACTION);
}

avp_ftype get_ftype(char *fpath){
	 char *dot = strrchr(fpath, '.');
	 if(!dot || dot == fpath) {
	         return AVP_FTYPE_ERR;
	     }
	 if(strcasecmp(dot, ".bin") == 0){
		 return AVP_FTYPE_BIN;
	 }
	 else if(strcasecmp(dot, ".hex") == 0){
		 return AVP_FTYPE_HEX;
	 }
	 else if(strcasecmp(dot, ".txt") == 0){
		 return AVP_FTYPE_TXT;
	 }
	 else if(strcasecmp(dot, ".cfg") == 0){
		 return AVP_FTYPE_CFG;
	 }
	 else{
		 return AVP_FTYPE_ERR;
	 }
}