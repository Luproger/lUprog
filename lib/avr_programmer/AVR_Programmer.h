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


#define PGM_ENABLE_1 0xAC
#define PGM_ENABLE_2 0x53
#define SIGNATURE_READ 0x30
#define CHIP_ERASE 0xAC
#define ERASE_PARAM 0x80
#define LOAD_EXT_ADDR 0x4D
#define WRITE_LOW_BYTE 0x40
#define WRITE_HIGH_BYTE 0x48
#define COMMIT_PAGE 0x4C
#define READ_FLASH 0x20
#define WRITE_LOCK 0xAC
#define LOCK_PARAM 0xE0
#define WRITE_FUSE 0xAC
#define WRITE_FUSE_HIGH 0xA8
#define WRITE_EXT_FUSE 0xA4

typedef struct{
	SPI_HandleTypeDef *hspi;
	uint16_t CS_Pin;
	GPIO_TypeDef *CS_Port;
}avr_prog_init_t;

typedef struct{
	uint32_t baudRate;
}avr_prog_uart_settings_t;

typedef struct{
	bool auto_sck_speed;
	uint16_t sck_div;
}avr_prog_spi_settings_t;

typedef union{
	avr_prog_spi_settings_t spi;
	avr_prog_uart_settings_t uart;
}avr_prog_protocol_u;

typedef enum{
	AVR_PROG_SPI,
	AVR_PROG_UART
}avr_prog_protocol_e;

typedef struct {
	avr_prog_protocol_e name;
	avr_prog_protocol_u settings;
}avr_prog_protocol_t;

typedef struct {
	char *path;
	FIL *file;
	mcu_t *mcu;
	avr_prog_protocol_t *protocol;

} avr_prog_param_t;


typedef enum{
	A_OK,
	A_ERR_NULL_MCU = -1,
	A_ERR_NULL_PATH = -2,
	A_ERR_NULL_PROTOCOL =-3,
	A_ERR_FILE_NOT_CLOSED = -4,
	A_ERR_SPI_CONFIG = -5,
	A_ERR_OPEN_FILE = -6,
	//FR_DISK_ERR = -7,
	//.....
	//FR_INVALID_PARAMETER =-26


} avr_progStatus;

void AVR_Programmer_Init(avr_prog_init_t *prog, SPI_HandleTypeDef *hspi, uint16_t CS_Pin, GPIO_TypeDef *CS_Port);

avr_progStatus AVR_WarmUpToProgram(avr_prog_param_t *param,char *fname,mcu_t *mcu,avr_prog_protocol_t *protocol);

void spi_enable(avr_prog_init_t *prog, avr_prog_param_t *params);
void spi_disable(avr_prog_init_t *prog);
uint8_t spi_send_byte(SPI_HandleTypeDef *hspi, uint8_t byte);
uint8_t spi_send_cmd(SPI_HandleTypeDef *hspi, uint8_t a, uint8_t b, uint8_t c, uint8_t d);

void flash_AVR(avr_prog_init_t *prog, avr_prog_param_t *params);

bool enterProgrammingMode(avr_prog_init_t *prog, avr_prog_param_t *params);
void leaveProgrammingMode();

bool checkSignature();
void chipErase(SPI_HandleTypeDef *hspi, uint8_t chip_erase_delay);

void f_write_page(SPI_HandleTypeDef *hspi, uint32_t *page_word_addr);
void f_commit(SPI_HandleTypeDef *hspi, uint32_t addr);
uint8_t read_flash(uint8_t hilo, uint32_t word);
bool f_bsy_check();

#endif /* AVR_PROGRAMMER_AVR_PROGRAMMER_H_ */
