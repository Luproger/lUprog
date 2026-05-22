/*
 * avp_internal.h
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_AVP_INTERNAL_H_
#define AVR_PROGRAMMER_AVP_INTERNAL_H_

#include "AVR_Programmer.h"
#include "debug.h"
#include "error_list.h"
#include "ff.h"

#include <string.h>
#include <stdint.h>
#include "stdbool.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_PAGE_SIZE_BYTES 512

typedef enum {
	AVP_FTYPE_BIN,
	AVP_FTYPE_HEX,
	AVP_FTYPE_TXT,
	AVP_FTYPE_CFG,
	AVP_FTYPE_DEF,
	AVP_FTYPE_ERR
} avp_ftype;

typedef struct {
	// INIT
	avpFunc check_conf;
	avpFunc prog_init;
	void (*prog_deinit)(void);

	// FLASH
	avpFunc fl_Write;
	avpFunc fl_Read;
	avpFunc fl_Verify;

	// EEPROM
	avpFunc ee_Write;
	avpFunc ee_Read;
	avpFunc ee_Verify;

	// FUSEBIT
	avpFunc fb_Write;
	avpFunc fb_Read;
	avpFunc fb_Verify;

	// LOCKBIT
	avpFunc lb_Write;
	avpFunc lb_Read;
	avpFunc lb_Verify;

	// CONFIG
	avpFunc cfg_Write;
	avpFunc cfg_Read;
	avpFunc cfg_Verify;
}avr_prog_proto_t;

extern avp_spi_conf *spiConf;
extern avp_uart_conf *uartConf;

extern avp_action curAction;
extern const avp_init_t *avr_prog;
extern  avp_param_t *avp_curParam;

extern bool AVP_ERROR;
extern char errMessage[64];
extern const char* errType;

extern FIL firmwareFile;
extern avp_ftype firmwareFtype;
extern uint8_t flash_buf[MAX_PAGE_SIZE_BYTES];

extern uint32_t f_page_size_b;

void FAIL(const char* error_type, const char* fmt, ...);
void null_actFunc();

#endif /* AVR_PROGRAMMER_AVP_INTERNAL_H_ */
