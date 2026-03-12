/*
 * mcu.c
 *
 *  Created on: Dec 29, 2025
 *      Author: Luproger
 */

#include "mcu.h"
#include "stdlib.h"
#include "string.h"
#include "ff.h"
#include "debug.h"

FIL chipFile;
mcu_t_u current;
uint8_t chip_index;

#define CHIP_NANE_LEN 14
#define CHIP_LINE_SIZE 52


void openChipList(){
	FRESULT res;

	res = f_open(&chipFile, (char*) "system/LIST.TXT", FA_READ);
	if(res!=FR_OK){
		DEBUG_PRINTF("NOT OPEN FILE MCU LIST! \n");
	}
	DEBUG_PRINTF("Open file mcu list! \n");
	chip_index=0;
	readChip();
}
void closeChipList(){
	f_close(&chipFile);
}

uint8_t charToNum(char c){
	if(c>='0' && c<='9'){
		return c-'0';
	}else if(c=='A'){
		return 10;
	}else if(c=='B'){
		return 11;
	}else if(c=='C'){
		return 12;
	}else if(c=='D'){
		return 13;
	}else if(c=='E'){
		return 14;
	}else if(c=='F'){
		return 15;
	}
	return 0;
}

void readChip(){
	f_lseek(&chipFile, chip_index*CHIP_LINE_SIZE);
	uint8_t buf[CHIP_LINE_SIZE];
	UINT readByte;
	FRESULT res;

	res = f_read(&chipFile, buf, CHIP_LINE_SIZE, &readByte);
	if(res!=FR_OK){
		DEBUG_PRINTF("NOT READ CHIP ON MCU LIST! \n");
	}
	int ptr = 0;
	for(uint8_t i =0; i < (CHIP_LINE_SIZE-CHIP_NANE_LEN)/2 ;i++){
		current.buf[ptr]=charToNum(buf[i*2])<<4 | charToNum(buf[i*2+1]);
		ptr++;
	}
	for(uint8_t i =(CHIP_LINE_SIZE-CHIP_NANE_LEN)-2; i < CHIP_LINE_SIZE ;i++){
			current.buf[ptr]=buf[i];
			ptr++;
	}
	DEBUG_PRINTF("Chip Read! \n");
}

mcu_t *getChip(){
	return &current.mcu;
}

void incChip(){
	if(chip_index<(chipFile.fsize/CHIP_LINE_SIZE)-1){
		chip_index++;
		readChip();
	}
}
void decChip(){
	if(chip_index >= 0){
		chip_index--;
		readChip();
	}
}
