/*
 * sd_driver.h
 *
 *  Created on: 4 May 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_SD_DRIVER_H_
#define AVR_PROGRAMMER_SD_DRIVER_H_
#include "avp_internal.h"

typedef bool (*sd_func_t)();

extern sd_func_t SD_transferFunc;

avp_ftype get_ftype(char *fpath);
void SD_SetFunc(avp_ftype ftype);


#endif /* AVR_PROGRAMMER_SD_DRIVER_H_ */
