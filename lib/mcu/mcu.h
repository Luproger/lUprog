/*
 * mcu.h
 *
 *  Created on: Dec 29, 2025
 *      Author: Luproger
 */

#ifndef MCU_H_
#define MCU_H_

#include "stdint.h"

typedef struct{
	uint8_t ee_reserved:1;
	uint8_t ee_data_polling:1;
	uint8_t ee_bsy_check:1;
	uint8_t ee_page_programming:1;
	uint8_t extended_memory:1;
	uint8_t data_polling:1;
	uint8_t bsy_check:1;
	uint8_t page_programming:1;
}program_mode_t;

//typedef union{
//	program_mode_t bits;
//	uint8_t num;
//}program_mode_t_u;

typedef struct{
	uint8_t sig[3];
	uint8_t defFuse[3];
	program_mode_t mrogramMetod;
	uint8_t flash_page_size;
	uint16_t number_of_flash_pages;
	uint8_t eeprom_page_size;
	uint16_t number_of_eeprom_pages;
	uint8_t fusebyte_prog_delay;
	uint8_t flash_prog_delay;
	uint8_t eeprom_prog_delay;
	uint8_t chip_erase_delay;
	uint8_t reserved_byte;
	uint8_t chip_name[14];
}mcu_t;

typedef union{
	mcu_t mcu;
	uint8_t buf[sizeof(mcu_t)];
}mcu_t_u;

void decChip();
void incChip();
void readChip();
void openChipList();
void closeChipList();

#endif /* MCU_H_ */
