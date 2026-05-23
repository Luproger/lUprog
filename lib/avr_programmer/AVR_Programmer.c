/*
 * AVR_Programmer.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Luproger
 */

#include "avp_internal.h"
#include "sd_driver.h"
#include "spi_driver.h"
#include "uart_driver.h"

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
	spi_prog_deinit();

	DEBUG_PRINTF("AVP INIT: OK\n");
	return 1;
}

void Close_Session(){
	// Закрыть пасть либералу!
	f_close(&firmwareFile);
	

	if(AVP_ERROR){
		avr_prog->err_cb(errType, errMessage);
		AVP_ERROR = 0;
	}
	else{
		avr_prog->sucs_cb();
	}

	DEBUG_PRINTF("\n----------------- SESSION CLOSED ----------------- \n");

}

void Init_Session(avp_param_t *_param){

	if(avr_prog == NULL){
		FAIL(AVP_ERR_INIT, AVP_ERR_MISSING_INIT);
		return;
	}
	if(_param->mcu==NULL){
		FAIL(AVP_ERR_INIT, AVP_ERR_NULL_MCU);
		return;
	}
	if(firmwareFile.fs != NULL){
		FAIL(AVP_ERR_INIT, AVP_ERR_FILE_NOT_CLOSED);
		return;
	}
	if(curDrv == NULL){
		FAIL(AVP_ERR_INIT, AVP_ERR_NULL_PROTO);
		return;
	}

	// Проверяем файл
	if(curAction == ACT_FB_DEFAULT){
		firmwareFtype = AVP_FTYPE_DEF;
	}else{

		if(_param->path==NULL || strlen(_param->path)==0){
			FAIL(AVP_ERR_INIT, AVP_ERR_NULL_PATH);
			return;
		}

		firmwareFtype = get_ftype(_param->path);

		if(firmwareFtype == AVP_FTYPE_ERR){
			FAIL(AVP_ERR_INIT, AVP_ERR_INVALID_FTYPE);
			return;
		}

		// Проверяем на поддержку тип файла и устанавливаем функцию трансфер
		SD_SetFunc(firmwareFtype);
		if(AVP_ERROR) return;

		// Открываем
		FRESULT res = f_open(&firmwareFile, _param->path, SD_fileMode);

		if(res!= FR_OK){
			FAIL(AVP_ERR_SD, AVP_SD_ERRORS[res - 1]);
			return;
		}
	}

	// Проверяем протокол
	curDrv->check_conf();
	if(AVP_ERROR) return;

	// Устанавливаем параметры
	avp_curParam = _param;
	f_page_size_b = avp_curParam->mcu->flash_page_size * 2;
	memset(errMessage, 0, sizeof(errMessage));

	DEBUG_PRINTF("INIT SESSION: OK\n");
	DEBUG_PRINTF("\n----------------- SESSION OPEN ----------------- \n\n");

}

void AVP_Execute(avp_action action, avp_param_t *_param){

	curAction = action;
	Init_Session(_param);
	if(AVP_ERROR) {
		DEBUG_PRINTF("INIT SESSION: ERR");
		Close_Session();
		return;
	}

	// Инициализируем программирование
	curDrv->prog_init();
	if(!AVP_ERROR){

		// Выполняем action
		switch(curAction){

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
	Close_Session();
}

void AVP_Set_SPI(avp_spi_conf *conf){
	curDrv = &spi_driver;
	spiConf = conf;
}
void AVP_Set_UART(avp_uart_conf *conf){
	uartConf =  conf;
	curDrv = &uart_driver;
}
