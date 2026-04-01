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
	AVP_OK,

	// INIT AVR_PROGRAMMER
	AVP_ERR_INIT_CONF,
	AVP_ERR_MISSING_INIT,

	// INIT SESSION
	AVP_ERR_NULL_MCU,
	AVP_ERR_NULL_PATH,
	AVP_ERR_NULL_PROTO,
	AVP_ERR_FILE_NOT_CLOSED,
	AVP_ERR_WRONG_FILE_FORMAT,
	AVP_ERR_SPI_BOUDRATE,

	AVP_ERR_WRONG_ACTION,

	// SD CARD ERROR
	AVP_POINT_SD_CARD_ERR,

	AVP_FR_DISK_ERR,
	AVP_FR_INT_ERR,
	AVP_FR_NOT_READY,
	AVP_FR_NO_FILE,
	AVP_FR_NO_PATH,
	AVP_FR_INVALID_NAME,
	AVP_FR_DENIED,
	AVP_FR_EXIST,
	AVP_FR_INVALID_OBJECT,
	AVP_FR_WRITE_PROTECTED,
	AVP_FR_INVALID_DRIVE,
	AVP_FR_NOT_ENABLED,
	AVP_FR_NO_FILESYSTEM,
	AVP_FR_MKFS_ABORTED,
	AVP_FR_TIMEOUT,
	AVP_FR_LOCKED,
	AVP_FR_NOT_ENOUGH_CORE,
	AVP_FR_TOO_MANY_OPEN_FILES,
	AVP_FR_INVALID_PARAMETER,

	// PROGRAMMING ERROR

	AVP_ERR_ENTER_PMODE,
	AVP_ERR_CHECK_SIG,


} avp_status;
extern const char *status_mes[];

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
typedef avp_status (*avrFunc)(void);
typedef void (*errCalback)(avp_status err, char* message);

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

avp_status AVP_Init(const avp_init_t *avrprog);
void AVP_Set_SPI(avp_spi_conf *conf);
void AVP_Set_UART(avp_uart_conf *conf);
avp_status AVP_Execute(avp_action action, avp_param_t *_param);

avp_status Init_Session(avp_action action, avp_param_t *_param);
avp_status Close_Session();

avp_status null_actFunc();

// INIT
avp_status spi_check_conf();
avp_status spi_prog_init();
void spi_prog_deinit();

// FLASH
avp_status spi_fl_Write();
avp_status spi_fl_Read();
avp_status spi_fl_Verify();

// EEPROM
avp_status spi_ee_Write();
avp_status spi_ee_Read();
avp_status spi_ee_Verify();

// FUSEBIT
avp_status spi_fb_Write();
avp_status spi_fb_Read();
avp_status spi_fb_Verify();

// LOCKBIT
avp_status spi_lb_Write();
avp_status spi_lb_Read();
avp_status spi_lb_Verify();

// CONFIG
avp_status spi_cfg_Write();
avp_status spi_cfg_Read();
avp_status spi_cfg_Verify();


#endif /* AVR_PROGRAMMER_AVR_PROGRAMMER_H_ */
