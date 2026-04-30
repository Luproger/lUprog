/*
 * AVR_Programmer.h
 *
 *  Created on: Feb 24, 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_AVR_PROGRAMMER_H_
#define AVR_PROGRAMMER_AVR_PROGRAMMER_H_

#include "stm32f1xx_hal.h"
#include "mcu.h"

#include <stdbool.h>
#include <stdint.h>


typedef enum{
	// FLASH
	ACT_FL_WRITE,
	ACT_FL_READ,
	ACT_FL_VERIFY,

	// EEPROM
	ACT_EE_WRITE,
	ACT_EE_READ,
	ACT_EE_VERIFY,

	// FUSEBIT
	ACT_FB_WRITE,
	ACT_FB_DEFAULT,
	ACT_FB_READ,
	ACT_FB_VERIFY,

	// LOCKBIT
	ACT_LB_WRITE,
	ACT_LB_READ,
	ACT_LB_VERIFY,

	// CONFIG
	ACT_CFG_WRITE,
	ACT_CFG_READ,
	ACT_CFG_VERIFY

	// BOOTLOADER
}avp_action;

typedef void (*exCallback)();
typedef void (*avpFunc)(void);
typedef void (*errCalback)(char* message);

typedef struct{
	exCallback prog_cb;
	errCalback err_cb;
	SPI_HandleTypeDef *hspi;
	uint16_t CS_Pin;
	GPIO_TypeDef *CS_Port;

}avp_init_t;

typedef struct{
	uint32_t baudRate;
}avp_uart_conf;

typedef struct{
	bool sck_auto;
	uint32_t sck_div;
}avp_spi_conf;

typedef struct {
	char *path;
	mcu_t *mcu;
} avp_param_t;

bool AVP_Init(const avp_init_t *avrprog);
void AVP_Set_SPI(avp_spi_conf *conf);
void AVP_Set_UART(avp_uart_conf *conf);
void AVP_Execute(avp_action action, avp_param_t *_param);

#endif /* AVR_PROGRAMMER_AVR_PROGRAMMER_H_ */
