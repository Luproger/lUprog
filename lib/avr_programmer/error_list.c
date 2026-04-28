/*
 * error_list.c
 *
 *  Created on: 24 Apr 2026
 *      Author: Luproger
 */
#include "error_list.h"
    
	// INIT SESSION
    const char* const AVP_ERR_MISSING_INIT = "INIT ERR: MISSING_INIT";
	const char* const AVP_ERR_NULL_MCU = "INIT ERR: NULL_MCU";
	const char* const AVP_ERR_NULL_PATH = "INIT ERR: NULL_PATH";
	const char* const AVP_ERR_NULL_PROTO = "INIT ERR: NULL_PROTO";
	const char* const AVP_ERR_FILE_NOT_CLOSED = "INIT ERR: FILE_NOT_CLOSED";
	const char* const AVP_ERR_FILE_FORMAT = "INIT ERR: INVALID_FILE_FORMAT";
	const char* const AVP_ERR_SPI_BOUDRATE = "INIT ERR: INVALID_SPI_BOUDRATE";
	const char* const AVP_ERR_ACTION = "INIT ERR: INVALID_ACTION";

    
	// SD CARD ERROR
    const char* const AVP_SD_ERRORS[] = {
        "SD ERR: FR_DISK_ERR",
        "SD ERR: FR_INT_ERR",
        "SD ERR: FR_NOT_READY",
        "SD ERR: FR_NO_FILE",
        "SD ERR: FR_NO_PATH",
        "SD ERR: FR_INVALID_NAME",
        "SD ERR: FR_DENIED",
        "SD ERR: FR_EXIST",
        "SD ERR: FR_INVALID_OBJECT",
        "SD ERR: FR_WRITE_PROTECTED",
        "SD ERR: FR_INVALID_DRIVE",
        "SD ERR: FR_NOT_ENABLED",
        "SD ERR: FR_NO_FILESYSTEM",
        "SD ERR: FR_MKFS_ABORTED",
        "SD ERR: FR_TIMEOUT",
        "SD ERR: FR_LOCKED",
        "SD ERR: FR_NOT_ENOUGH_CORE",
        "SD ERR: FR_TOO_MANY_OPEN_FILES",
        "SD ERR: FR_INVALID_PARAMETER"
    };


	// PROGRAMMING ERROR
	const char* const AVP_ERR_ENTER_PMODE = "PROG ERR: ERR_ENTER_PMODE";
	const char* const AVP_ERR_CHECK_SIG = "PROG ERR: ERR_CHECK_SIG\nREAD SIG IS: 0x%02X 0x%02X 0x%02X";
