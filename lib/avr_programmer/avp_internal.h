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



#define ACTION_MAX 16
#define MAX_PAGE_SIZE_BYTES 512

typedef enum {
	AVP_FTYPE_BIN = (1 << 0), // 0x01
	AVP_FTYPE_HEX = (1 << 1), // 0x02
	AVP_FTYPE_TXT = (1 << 2), // 0x04
	AVP_FTYPE_CFG = (1 << 3), // 0x08
	AVP_FTYPE_DEF = (1 << 4), // 0x10
	AVP_FTYPE_ERR = (1 << 5)
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

extern const uint8_t file_supp[ACTION_MAX];
extern const avp_init_t *avr_prog;
extern avp_param_t *param;

extern bool AVP_ERROR;
extern char errMessage[100];

extern FIL firmwareFile;
extern avp_ftype firmwareFtype;
extern uint8_t flash_buf[MAX_PAGE_SIZE_BYTES];

extern uint32_t f_page_size_b;

void FAIL(const char* fmt, ...);
void null_actFunc();
avp_ftype get_ftype(char *fpath);

#endif /* AVR_PROGRAMMER_AVP_INTERNAL_H_ */
