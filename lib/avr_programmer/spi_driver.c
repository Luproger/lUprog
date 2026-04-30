/*
 * spi_driver.c
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */
#include "spi_driver.h"
#include "avp_internal.h"

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

void spi_enable(){
	avr_prog->hspi->Init.Mode = SPI_MODE_MASTER;
	avr_prog->hspi->Init.Direction = SPI_DIRECTION_2LINES;
	avr_prog->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	avr_prog->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	avr_prog->hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	avr_prog->hspi->Init.NSS = SPI_NSS_SOFT;
	avr_prog->hspi->Init.BaudRatePrescaler = spiConf->sck_div;
	avr_prog->hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	avr_prog->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	avr_prog->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	avr_prog->hspi->Init.CRCPolynomial = 10;

	HAL_SPI_Init(avr_prog->hspi);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(avr_prog->CS_Port, avr_prog->CS_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = avr_prog->CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(avr_prog->CS_Port, &GPIO_InitStruct);

}
void spi_disable(){
	// Отключаем SPI
	HAL_SPI_DeInit(avr_prog->hspi);

	// Отключаем CS
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = avr_prog->CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(avr_prog->CS_Port, &GPIO_InitStruct);
}

uint8_t spi_send_cmd(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
	uint8_t res;
	HAL_SPI_Transmit(avr_prog->hspi, &a, 1, 1);
	HAL_SPI_Transmit(avr_prog->hspi, &b, 1, 1);
	HAL_SPI_Transmit(avr_prog->hspi, &c, 1, 1);
	HAL_SPI_TransmitReceive(avr_prog->hspi, &d, &res, 1, 10);
	return res;
}

uint8_t spi_send_byte(uint8_t byte){
	uint8_t res;
	HAL_SPI_TransmitReceive(avr_prog->hspi, &byte, &res, 1, 10);
	return res;
}

void enterPMode(){

	if(spiConf->sck_auto){

	}

	spi_enable();
	spi_send_byte(0X00);
	HAL_Delay(5);

	HAL_GPIO_WritePin(avr_prog->CS_Port, avr_prog->CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(25);


	spi_send_byte(PGM_ENABLE_1);
	spi_send_byte(PGM_ENABLE_2);
	uint8_t response = spi_send_byte(0x00);
	spi_send_byte(0X00);

	if (response != 0x53)
	  {
		FAIL(AVP_ERR_ENTER_PMODE);
	    return;
	  }

	DEBUG_PRINTF("ENTER_PMODE\n");
}

void checkSignature(){

	uint8_t sig1 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x00, 0x00);
	uint8_t sig2 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x01, 0x00);
	uint8_t sig3 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x02, 0x00);

	if (sig1 == param->mcu->sig[0]
		&& sig2 == param->mcu->sig[1]
		&& sig3 == param->mcu->sig[2]){
		DEBUG_PRINTF("CHECK SIGNATURE OK: ");
		DEBUG_PRINTF("0x%02X 0x%02X 0x%02X", sig1, sig2, sig3);
		DEBUG_PRINTF("\n");
		return;
	}

	FAIL(AVP_ERR_CHECK_SIG, sig1, sig2, sig3);
}

void chipErase(){
	spi_send_cmd(CHIP_ERASE, ERASE_PARAM, 0x00, 0x00);
	HAL_Delay(param->mcu->chip_erase_delay); // Время стирания
}

// INIT
void spi_check_conf(){
	if(!spiConf->sck_auto
	&& !IS_SPI_BAUDRATE_PRESCALER(spiConf->sck_div)){
		FAIL(AVP_ERR_SPI_BOUDRATE);
	}
}

void spi_prog_init(){

	enterPMode(); // Режим программирования
	if(AVP_ERROR) return;

	checkSignature(); // Проверяем сигнатуру
	if(AVP_ERROR) return;

	chipErase(); // Стираем чип

	DEBUG_PRINTF("PROGRAM MODE INIT\n");

}

void spi_prog_deinit(){
	HAL_GPIO_WritePin(avr_prog->CS_Port, avr_prog->CS_Pin, GPIO_PIN_SET);
	spi_disable();
	DEBUG_PRINTF("PROGRAM MODE DEINIT\n");
}

// FLASH
void spi_fl_Write(){}
void spi_fl_Read(){}
void spi_fl_Verify(){}

// EEPROM
void spi_ee_Write(){}
void spi_ee_Read(){}
void spi_ee_Verify(){}

// FUSEBIT
void spi_fb_Write(){}
void spi_fb_Read(){}
void spi_fb_Verify(){}

// LOCKBIT
void spi_lb_Write(){}
void spi_lb_Read(){}
void spi_lb_Verify(){}

// CONFIG
void spi_cfg_Write(){}
void spi_cfg_Read(){}
void spi_cfg_Verify(){}