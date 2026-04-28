/*
 * AVR_Programmer.h
 *
 *  Created on: Feb 24, 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_AVR_PROGRAMMER_H_
#define AVR_PROGRAMMER_AVR_PROGRAMMER_H_

#include "ff.h"
#include "mcu.h"

#include "stm32f1xx_hal_spi.h"
#include "stm32f1xx_hal_gpio.h"

#include "stdbool.h"
#include <stdio.h>
#include <string.h>
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
typedef void (*avrFunc)(void);
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

void Init_Session(avp_action action, avp_param_t *_param);
void Close_Session();

void null_actFunc();

// INIT
void spi_check_conf();
void spi_prog_init();
void spi_prog_deinit();

// FLASH
void spi_fl_Write();
void spi_fl_Read();
void spi_fl_Verify();

// EEPROM
void spi_ee_Write();
void spi_ee_Read();
void spi_ee_Verify();

// FUSEBIT
void spi_fb_Write();
void spi_fb_Read();
void spi_fb_Verify();

// LOCKBIT
void spi_lb_Write();
void spi_lb_Read();
void spi_lb_Verify();

// CONFIG
void spi_cfg_Write();
void spi_cfg_Read();
void spi_cfg_Verify();


#endif /* AVR_PROGRAMMER_AVR_PROGRAMMER_H_ */
