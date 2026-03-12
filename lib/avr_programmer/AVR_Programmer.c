/*
 * AVR_Programmer.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Luproger
 */
#include "AVR_Programmer.h"
#include "stm32f1xx_hal.h"
#include "debug.h"

#define PAGE_SIZE_BYTES 512
uint8_t flash_buf[PAGE_SIZE_BYTES];

uint8_t f_page_size_b;
uint16_t f_cur_page;


void spi_enable(avr_prog_init_t *prog, avr_prog_param_t *params){
	DEBUG_PRINTF("Тест");
	prog->hspi->Init.Mode = SPI_MODE_MASTER;
	prog->hspi->Init.Direction = SPI_DIRECTION_2LINES;
	prog->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	prog->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	prog->hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	prog->hspi->Init.NSS = SPI_NSS_SOFT;
	prog->hspi->Init.BaudRatePrescaler = params->protocol->settings.spi.sck_div;
	prog->hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	prog->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	prog->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	prog->hspi->Init.CRCPolynomial = 10;

	HAL_SPI_Init(prog->hspi);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(prog->CS_Port, prog->CS_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = prog->CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(prog->CS_Port, &GPIO_InitStruct);

}
void spi_disable(avr_prog_init_t *prog){
	// Отключаем SPI
	HAL_SPI_DeInit(prog->hspi);

	// Отключаем CS
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = prog->CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(prog->CS_Port, &GPIO_InitStruct);
}

uint8_t spi_send_byte(SPI_HandleTypeDef *hspi, uint8_t byte){
	uint8_t res;
	HAL_SPI_TransmitReceive(hspi, &byte, &res, 1, 10);
	return res;
}

uint8_t spi_send_cmd(SPI_HandleTypeDef *hspi, uint8_t a, uint8_t b, uint8_t c, uint8_t d){
	uint8_t res;
	HAL_SPI_Transmit(hspi, &a, 1, 1);
	HAL_SPI_Transmit(hspi, &b, 1, 1);
	HAL_SPI_Transmit(hspi, &c, 1, 1);
	HAL_SPI_TransmitReceive(hspi, &d, &res, 1, 10);
	return res;
}

void AVR_Programmer_Init(avr_prog_init_t *prog, SPI_HandleTypeDef *hspi, uint16_t CS_Pin, GPIO_TypeDef *CS_Port){
	prog->hspi = hspi;
	prog->CS_Port = CS_Port;
	prog->CS_Pin= CS_Pin;

	spi_disable(prog);

}

avr_progStatus AVR_WarmUpToProgram(avr_prog_param_t *param,char *fname,mcu_t *mcu,avr_prog_protocol_t *protocol){
	if(mcu==NULL){
		return A_ERR_NULL_MCU;
	}
	if(fname==NULL ||strlen(fname)==0){
		return A_ERR_NULL_PATH;
	}
	if(protocol==NULL){
		return A_ERR_NULL_PROTOCOL;
	}
	if(param->file!=NULL){
		return A_ERR_FILE_NOT_CLOSED;
	}

	FRESULT res = f_open(param->file, fname, FA_READ);
	if(res!=0){
		return (avr_progStatus)-(res+6);
	}

	param->protocol = protocol;
	param->mcu = mcu;

	f_page_size_b = param->mcu->flash_page_size * 2;

	f_cur_page = 0;

	return A_OK;
}
void fill_flash_buf(FIL *file){
	static UINT br;

	FRESULT res = f_read(file, flash_buf, f_page_size_b, &br);

	if(res != FR_OK){
	  // TODO return
	}
	if(br != f_page_size_b){ // Если файл закончился

		for(uint16_t i = br; i < f_page_size_b; i++){
			flash_buf[i] = 0xFF;
		}
		// TODO return
	}

	// TODO return
}

void programmFileBin(avr_prog_init_t *prog, avr_prog_param_t *param){
	uint32_t word_addr = f_cur_page * param->mcu->number_of_flash_pages;

	fill_flash_buf(param->file);
	// TODO проверки на FR_State

	if(f_cur_page < param->mcu->number_of_flash_pages){

		f_write_page(prog->hspi, &word_addr);

		if(param->mcu->mrogramMetod.bsy_check == 1){
			while(!f_bsy_check()); // TODO

		}else{
			HAL_Delay(param->mcu->flash_prog_delay);
		}

		// TODO проверки

		f_cur_page++;

		// TODO проверка на окончание файла


	}else{
		// TODO return
	}



};

void f_write_page(SPI_HandleTypeDef *hspi, uint32_t *page_word_addr){
	uint16_t word = 0;

	int x = 0;
	while (x < f_page_size_b)
	{
	// Пишем младший и старший байты слова
	spi_send_cmd(hspi, WRITE_LOW_BYTE, word >> 8, word & 0xFF, flash_buf[x++]);
	spi_send_cmd(hspi, WRITE_HIGH_BYTE, word >> 8, word & 0xFF, flash_buf[x++]);

	word++;
	}
	f_commit(hspi, *page_word_addr);
}

void f_commit(SPI_HandleTypeDef *hspi, uint32_t addr){
	spi_send_cmd(hspi, COMMIT_PAGE, addr >> 8, addr & 0xFF, 0);
}

bool f_bsy_check(){
	return 1;
}
bool enterProgrammingMode(avr_prog_init_t *prog, avr_prog_param_t *params){
//	if(params->protocol->settings.spi.auto_sck_speed){
//
//	}
	spi_enable(prog, params);

	HAL_GPIO_WritePin(prog->CS_Port, prog->CS_Pin, GPIO_PIN_SET);
	HAL_Delay(20);

	spi_send_byte(prog->hspi, PGM_ENABLE_1);
	spi_send_byte(prog->hspi, PGM_ENABLE_2);
	uint8_t response = spi_send_byte(prog->hspi, 0x00);
	spi_send_byte(prog->hspi, 0X00);

	if (response != 0x53)
	  {
	    return false;
	  }

	return true;
}

bool checkSignature(SPI_HandleTypeDef *hspi, uint8_t *tarSig){
	uint8_t sig1 = spi_send_cmd(hspi, SIGNATURE_READ, 0x00, 0x00, 0x00);
	uint8_t sig2 = spi_send_cmd(hspi, SIGNATURE_READ, 0x00, 0x00, 0x00);
	uint8_t sig3 = spi_send_cmd(hspi, SIGNATURE_READ, 0x00, 0x00, 0x00);

	if (sig1 == tarSig[0] && sig2 == tarSig[1] && sig3 == tarSig[2])
	  {
	    return true;
	  }

	  return false;

}

void chipErase(SPI_HandleTypeDef *hspi, uint8_t chip_erase_delay){
	spi_send_cmd(hspi, CHIP_ERASE, ERASE_PARAM, 0x00, 0x00);
	HAL_Delay(chip_erase_delay); // Время стирания

}


