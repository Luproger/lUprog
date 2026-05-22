/*
 * spi_driver.c
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */
#include "spi_driver.h"
#include "sd_driver.h"
#include "avp_internal.h"

#define PGM_ENABLE_1 0xAC
#define PGM_ENABLE_2 0x53
#define SIGNATURE_READ 0x30
#define CHIP_ERASE 0xAC
#define ERASE_PARAM 0x80
#define LOAD_EXT_ADDR 0x4D
#define WRITE_LOW_BYTE 0x40
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
    uint8_t tx_buf[4] = {a, b, c, d};
    uint8_t rx_buf[4] = {0};

    HAL_SPI_TransmitReceive(avr_prog->hspi, tx_buf, rx_buf, 4, 100);
	return rx_buf[3];
}

uint8_t spi_send_byte(uint8_t byte){
	uint8_t res;
	HAL_SPI_TransmitReceive(avr_prog->hspi, &byte, &res, 1, 100);
	return res;
}

void flWriteWord(uint8_t hilo, uint32_t word, uint8_t data){
	spi_send_cmd(WRITE_LOW_BYTE + 8 * hilo, word >> 8, word & 0xFF, data);
}
void flPageCommit(uint32_t addr){
	spi_send_cmd(COMMIT_PAGE, addr >> 8, addr & 0xFF, 0);
}

uint8_t flReadByte(uint8_t hilo, uint32_t word)
{
  return spi_send_cmd(READ_FLASH + hilo * 8, word >> 8, word & 0xFF, 0);
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
		FAIL(AVP_ERR_PROG, AVP_ERR_ENTER_PMODE);
	    return;
	  }

	DEBUG_PRINTF("ENTER_PMODE\n");
}

void checkSignature(){

	uint8_t sig1 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x00, 0x00);
	uint8_t sig2 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x01, 0x00);
	uint8_t sig3 = spi_send_cmd(SIGNATURE_READ, 0x00, 0x02, 0x00);

	if (sig1 == avp_curParam->mcu->sig[0]
		&& sig2 == avp_curParam->mcu->sig[1]
		&& sig3 == avp_curParam->mcu->sig[2]){
		DEBUG_PRINTF("CHECK SIGNATURE OK: ");
		DEBUG_PRINTF("0x%02X 0x%02X 0x%02X", sig1, sig2, sig3);
		DEBUG_PRINTF("\n");
		return;
	}

	FAIL(AVP_ERR_PROG, AVP_ERR_CHECK_SIG, sig1, sig2, sig3);
}

void chipErase(){
	spi_send_cmd(CHIP_ERASE, ERASE_PARAM, 0x00, 0x00);
	HAL_Delay(avp_curParam->mcu->chip_erase_delay); // Время стирания
}

// INIT
void spi_check_conf(){
	if(!spiConf->sck_auto
	&& !IS_SPI_BAUDRATE_PRESCALER(spiConf->sck_div)){
		FAIL(AVP_ERR_PROG, AVP_ERR_SPI_BOUDRATE);
	}
}

void spi_prog_init(){

	enterPMode(); // Режим программирования
	if(AVP_ERROR) return;

	checkSignature(); // Проверяем сигнатуру
	if(AVP_ERROR) return;

	DEBUG_PRINTF("PROGRAM MODE INIT\n");

}

void spi_prog_deinit(){
	HAL_GPIO_WritePin(avr_prog->CS_Port, avr_prog->CS_Pin, GPIO_PIN_SET);
	spi_disable();
	DEBUG_PRINTF("PROGRAM MODE DEINIT\n");
}

// FLASH
void spi_fl_Write(){
	uint16_t curPage;
	uint32_t page_word_addr;
  	uint16_t word;

	DEBUG_PRINTF("CHIP ERASE\n");
	chipErase(); // Стираем чип

	DEBUG_PRINTF("START WRITE FLASH\n");
	for(curPage = 0; curPage < avp_curParam->mcu->number_of_flash_pages; curPage++){
		// Читаем в буфер и проверяем доступность файла
		if(!SD_transferFunc()) {
			if(AVP_ERROR) break;

			DEBUG_PRINTF("FILE ENDED\n");
			break;
		}
		page_word_addr = curPage * avp_curParam->mcu->flash_page_size;

		for(word = 0; word < avp_curParam->mcu->flash_page_size; word++){
			flWriteWord(0, word, flash_buf[word * 2]);
			flWriteWord(1, word, flash_buf[word * 2 + 1]);
		}
		flPageCommit(page_word_addr);
		HAL_Delay(10); // ЭТО ОЧЕНЬ ВАЖНО ПОСЛЕ ЗАПИСИ, ПОДОЖДИ

		// ProgressBar
		avr_prog->prog_cb(avp_curParam->mcu->number_of_flash_pages, curPage);
	}
	DEBUG_PRINTF("END WRITE FLASH\n");
	
}
void spi_fl_Read(){
}
void spi_fl_Verify(){
	uint32_t word_addr = 0;
	uint16_t page_word = 0;

	uint8_t verify_buf[4];

	DEBUG_PRINTF("START VERIFY FLASH\n");
	
	while(word_addr < avp_curParam->mcu->flash_page_size * avp_curParam->mcu->number_of_flash_pages){
		// Читаем в буфер и проверяем доступность файла
		if(!SD_transferFunc()) {
			if(AVP_ERROR) return;

			FAIL(AVP_ERR_PROG, AVP_ERR_VERIFY, word_addr * 2, 0xFF, flReadByte(0, word_addr));
			DEBUG_PRINTF("FILE ENDED BEFORE FLASH!\n");
			return;
		}
		for(page_word = 0; page_word < avp_curParam->mcu->flash_page_size; word_addr++, page_word++){
			verify_buf[0] = flash_buf[page_word *2];		// low orig
			verify_buf[1] = flash_buf[page_word *2 + 1];	// high orig

			verify_buf[2] = flReadByte(0, word_addr);		// low read
			verify_buf[3] = flReadByte(1, word_addr);		// high read

			if(verify_buf[0] != verify_buf[2]){
				FAIL(AVP_ERR_PROG, AVP_ERR_VERIFY, word_addr * 2, verify_buf[0], verify_buf[2]);
				return;
			}
			if(verify_buf[1] != verify_buf[3]){
				FAIL(AVP_ERR_PROG, AVP_ERR_VERIFY, word_addr * 2 + 1, verify_buf[1], verify_buf[3]);
				return;
			}
		// ProgressBar
		//avr_prog->prog_cb(avp_curParam->mcu->number_of_flash_pages, curPage);
		}

	}
	DEBUG_PRINTF("END VERIFY FLASH\n");
}

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
