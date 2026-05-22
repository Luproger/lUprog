/*
 * sd_driver.c
 *
 *  Created on: 4 May 2026
 *      Author: Luproger
 */
#include "sd_driver.h"
#include "stdint.h"

FRESULT res;
UINT br;
sd_func_t SD_transferFunc;


// GET
bool SD_GetFlBin(){
	res = f_read(&firmwareFile, flash_buf, f_page_size_b, &br); // TODO
	if(res != FR_OK) {
		FAIL(AVP_ERR_SD, AVP_SD_ERRORS[res - 1]);
		return 0;
	}

	if(br != f_page_size_b){
		if(br == 0) return 0;

		for (UINT i = br; i < f_page_size_b; i++){
			flash_buf[i] = 0xFF;
		}
	}
	return 1;
}
bool SD_GetFlHex(){}

bool SD_GetEeBin(){}
bool SD_GetEeHex(){}

bool SD_GetFbTxt(){}
bool SD_GetFbBin(){}
bool SD_GetFbDef(){}

bool SD_GetLbTxt(){}
bool SD_GetLbBin(){}

bool SD_GetCfg(){}


// SET
bool SD_SetFlBin(){}
bool SD_SetFlHex(){}

bool SD_SetEeBin(){}
bool SD_SetEeHex(){}

bool SD_SetFbTxt(){}
bool SD_SetFbBin(){}
bool SD_SetFbDef(){}

bool SD_SetLbTxt(){}
bool SD_SetLbBin(){}

bool SD_SetCfg(){}



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

void SD_SetFunc(avp_ftype ftype){
	SD_transferFunc = NULL;

	 switch(curAction){
	 // FLASH
	 case ACT_FL_WRITE:
	 case ACT_FL_VERIFY:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_GetFlBin;
	 	else if(ftype == AVP_FTYPE_HEX) SD_transferFunc = SD_GetFlHex;
	 	break;

	 case ACT_FL_READ:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_SetFlBin;
	 	else if(ftype == AVP_FTYPE_HEX) SD_transferFunc = SD_SetFlHex;
	 	break;

	 // EEPROM
	 case ACT_EE_WRITE:
	 case ACT_EE_VERIFY:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_GetEeBin;
	 	else if(ftype == AVP_FTYPE_HEX) SD_transferFunc = SD_GetEeHex;
	 	break;

	 case ACT_EE_READ:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_SetEeBin;
	 	else if(ftype == AVP_FTYPE_HEX) SD_transferFunc = SD_SetEeHex;
	 	break;

	 // FUSEBIT
	 case ACT_FB_WRITE:
	 case ACT_FB_VERIFY:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_GetFbBin;
	 	else if(ftype == AVP_FTYPE_TXT) SD_transferFunc = SD_GetFbTxt;
	 	break;

	 case ACT_FB_DEFAULT:
	 	if(ftype == AVP_FTYPE_DEF) SD_transferFunc = SD_GetFbDef;
	 	break;

	 case ACT_FB_READ:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_SetFbBin;
	 	else if(ftype == AVP_FTYPE_TXT) SD_transferFunc = SD_SetFbTxt;
	 	break;

	 // LOCKBIT
	 case ACT_LB_WRITE:
	 case ACT_LB_VERIFY:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_GetLbBin;
	 	else if(ftype == AVP_FTYPE_TXT) SD_transferFunc = SD_GetLbTxt;
	 	break;

	 case ACT_LB_READ:
	 	if(ftype == AVP_FTYPE_BIN) SD_transferFunc = SD_SetLbBin;
	 	else if(ftype == AVP_FTYPE_TXT) SD_transferFunc = SD_SetLbTxt;
	 	break;

	 // CONGIG
	 case ACT_CFG_WRITE:
	 case ACT_CFG_VERIFY:
	 	if(ftype == AVP_FTYPE_CFG) SD_transferFunc = SD_GetCfg;
	 	break;

	 case ACT_CFG_READ:
	 	if(ftype == AVP_FTYPE_CFG) SD_transferFunc = SD_SetCfg;
	 	break;
	 }

	if(SD_transferFunc == NULL) FAIL(AVP_ERR_INIT, AVP_ERR_INVALID_FTYPE);
}

