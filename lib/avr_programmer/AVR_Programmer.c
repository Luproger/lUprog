/*
 * AVR_Programmer.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Luproger
 */
#include "AVR_Programmer.h"
#include "stm32f1xx_hal.h"
#include "debug.h"
 #include "error_list.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>


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

#define ACTION_MAX 16

typedef enum {
	AVP_FTYPE_BIN = (1 << 0), // 0x01
	AVP_FTYPE_HEX = (1 << 1), // 0x02
	AVP_FTYPE_TXT = (1 << 2), // 0x04
	AVP_FTYPE_CFG = (1 << 3), // 0x08
	AVP_FTYPE_DEF = (1 << 4), // 0x10
	AVP_FTYPE_ERR = (1 << 5)
} avp_ftype;

static const uint8_t file_supp[ACTION_MAX] = {
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


typedef struct {
	// INIT
	avrFunc check_conf;
	avrFunc prog_init;
	void (*prog_deinit)(void);

	// FLASH
	avrFunc fl_Write;
	avrFunc fl_Read;
	avrFunc fl_Verify;

	// EEPROM
	avrFunc ee_Write;
	avrFunc ee_Read;
	avrFunc ee_Verify;

	// FUSEBIT
	avrFunc fb_Write;
	avrFunc fb_Read;
	avrFunc fb_Verify;

	// LOCKBIT
	avrFunc lb_Write;
	avrFunc lb_Read;
	avrFunc lb_Verify;

	// CONFIG
	avrFunc cfg_Write;
	avrFunc cfg_Read;
	avrFunc cfg_Verify;
}avr_prog_proto_t;

avr_prog_proto_t spi_driver = {
	.check_conf = spi_check_conf,
	.prog_init  = spi_prog_init,
	.prog_deinit= spi_prog_deinit,

	.fl_Write   = spi_fl_Write,
	.fl_Read    = spi_fl_Read,
	.fl_Verify  = spi_fl_Verify,

	.ee_Write   = spi_ee_Write,
	.ee_Read    = spi_fb_Read,
	.ee_Verify  = spi_ee_Verify,

	.fb_Write   = spi_fb_Write,
	.fb_Read    = spi_fb_Read,
	.fb_Verify  = spi_ee_Verify,

	.lb_Write   = spi_lb_Write,
	.lb_Read    = spi_lb_Read,
	.lb_Verify  = spi_lb_Verify,

	.cfg_Write  = spi_cfg_Write,
	.cfg_Read   = spi_cfg_Read,
	.cfg_Verify = spi_cfg_Verify
};

avr_prog_proto_t uart_driver = {
	.check_conf =  null_actFunc,
	.prog_init  =  null_actFunc,

	.fl_Write   =  null_actFunc,
	.fl_Read    =  null_actFunc,
	.fl_Verify  =  null_actFunc,

	.ee_Write   =  null_actFunc,
	.ee_Read    =  null_actFunc,
	.ee_Verify  =  null_actFunc,

	.fb_Write   =  null_actFunc,
	.fb_Read    =  null_actFunc,
	.fb_Verify  =  null_actFunc,

	.lb_Write   =  null_actFunc,
	.lb_Read    =  null_actFunc,
	.lb_Verify  =  null_actFunc,

	.cfg_Write  =  null_actFunc,
	.cfg_Read   =  null_actFunc,
	.cfg_Verify =  null_actFunc
};


avr_prog_proto_t *curDrv;

avp_spi_conf *spiConf;
avp_uart_conf *uartConf;

static const avp_init_t *avr_prog;
avp_param_t *param;

bool AVP_ERROR = 0;
char errMessage[100];

FIL firmwareFile = {0};

avp_ftype firmwareFtype;

uint8_t f_page_size_b;

#define MAX_PAGE_SIZE_BYTES 512
uint8_t flash_buf[MAX_PAGE_SIZE_BYTES];


void fail(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(errMessage, sizeof(errMessage), fmt, args);
	va_end(args);

	AVP_ERROR = 1; // ОЩИБКААААА
}

void null_actFunc(){
	fail(AVP_ERR_ACTION);
}

void AVP_Set_SPI(avp_spi_conf *conf){
	curDrv = &spi_driver;
	spiConf = conf;
}
void AVP_Set_UART(avp_uart_conf *conf){
	uartConf =  conf;
	curDrv = &uart_driver;
}
void spi_check_conf(){
	if(!spiConf->sck_auto
	&& !IS_SPI_BAUDRATE_PRESCALER(spiConf->sck_div)){
		fail(AVP_ERR_SPI_BOUDRATE);
	}
}

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

bool AVP_Init(const avp_init_t *avrprog){
	if(avrprog == NULL
	|| avrprog->prog_cb == NULL
	|| avrprog->err_cb == NULL
	|| avrprog->hspi == NULL
	|| avrprog->CS_Port == NULL
	|| !IS_GPIO_PIN(avrprog->CS_Pin)
	|| !IS_GPIO_ALL_INSTANCE(avrprog->CS_Port)){

		DEBUG_PRINTF("AVP INIT: ERR\n");
		return 0;
	}

	avr_prog = avrprog;
	spi_disable();

	DEBUG_PRINTF("AVP INIT: OK\n");
	return 1;
}

void AVP_Execute(avp_action action, avp_param_t *_param){
	// Готовимся к сессии
	Init_Session(action, _param);
	if(AVP_ERROR) {
		return Close_Session();
		DEBUG_PRINTF("INIT SESSION: ERR");
	}

	// Инициализируем программирование
	curDrv->prog_init();
	if(!AVP_ERROR){

		// Выполняем action
		switch(action){

		// FALSH
		case ACT_FL_WRITE:	 curDrv->fl_Write();   break;
		case ACT_FL_READ:  	 curDrv->fl_Read();    break;
		case ACT_FL_VERIFY:  curDrv->fl_Verify();  break;

		// EEPROM
		case ACT_EE_WRITE:   curDrv->ee_Write();   break;
		case ACT_EE_READ:    curDrv->ee_Read();    break;
		case ACT_EE_VERIFY:  curDrv->ee_Verify();  break;

		// FUSEBIT
		case ACT_FB_WRITE:   curDrv->fb_Write();   break;
		case ACT_FB_DEFAULT: curDrv->fb_Write();   break;
		case ACT_FB_READ:    curDrv->fb_Read();    break;
		case ACT_FB_VERIFY:  curDrv->fb_Verify();  break;

		// LOCKBIT
		case ACT_LB_WRITE:   curDrv->lb_Write();   break;
		case ACT_LB_READ:    curDrv->lb_Read();    break;
		case ACT_LB_VERIFY:  curDrv->lb_Verify();  break;

		// CONGIG
		case ACT_CFG_WRITE:  curDrv->cfg_Write();  break;
		case ACT_CFG_READ:   curDrv->cfg_Read();   break;
		case ACT_CFG_VERIFY: curDrv->cfg_Verify(); break;

		}
	}
	// Выключаем режим программирования
	curDrv->prog_deinit();

	return Close_Session();
}

void Close_Session(){
	// Ставим NULL на всякий
	curDrv = NULL;
	param = NULL;
	f_close(&firmwareFile);
	
	memset(errMessage, 0, sizeof(errMessage));

	if(AVP_ERROR){
		avr_prog->err_cb(errMessage);
		AVP_ERROR = 0;
	}
	DEBUG_PRINTF("\n----------------- SESSION CLOSED ----------------- \n");

}

void Init_Session(avp_action action, avp_param_t *_param){

	if(avr_prog == NULL){
		fail(AVP_ERR_MISSING_INIT);
		return;
	}
	if(_param->mcu==NULL){
		fail(AVP_ERR_NULL_MCU);
		return;
	}
	if(firmwareFile.fs != NULL){
		fail(AVP_ERR_FILE_NOT_CLOSED);
		return;
	}
	if(curDrv == NULL){
		fail(AVP_ERR_NULL_PROTO);
		return;
	}

	// Проверяем файл
	if(file_supp[action] & AVP_FTYPE_DEF){
		firmwareFtype = AVP_FTYPE_DEF;
	}else{

		if(_param->path==NULL || strlen(_param->path)==0){
			fail(AVP_ERR_NULL_PATH);
			return;
		}

		FRESULT res = f_open(&firmwareFile, _param->path, FA_READ);

		if(res!= FR_OK){
			fail(AVP_SD_ERRORS[(uint8_t) res]);
			return;
		}

		firmwareFtype = get_ftype(_param->path);

		if(!(file_supp[action] & firmwareFtype)){
			fail(AVP_ERR_FILE_FORMAT);
			return;
		}
	}

	// Проверяем протокол
	curDrv->check_conf();
	if(AVP_ERROR) return;

	// Устанавливаем параметры
	param = _param;
	f_page_size_b = param->mcu->flash_page_size * 2;

	DEBUG_PRINTF("INIT SESSION: OK\n");
	DEBUG_PRINTF("\n----------------- SESSION OPEN ----------------- \n\n");

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
		fail(AVP_ERR_ENTER_PMODE);
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

	fail(AVP_ERR_CHECK_SIG, sig1, sig2, sig3);
}

void chipErase(){
	spi_send_cmd(CHIP_ERASE, ERASE_PARAM, 0x00, 0x00);
	HAL_Delay(param->mcu->chip_erase_delay); // Время стирания
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
