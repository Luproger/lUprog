/*
 * error_list.c
 *
 *  Created on: 24 Apr 2026
 *      Author: Luproger
 */
#include "error_list.h"
    const char* const AVP_ERR_INIT = "INIT ERR: ";
    const char* const AVP_ERR_SD = "SD ERR: ";
    const char* const AVP_ERR_PROG = "PROG ERR: ";
	
    
	// INIT SESSION
    const char* const AVP_ERR_MISSING_INIT = "MISSING_INIT";
	const char* const AVP_ERR_NULL_MCU = "NULL_MCU";
	const char* const AVP_ERR_NULL_PATH = "NULL_PATH";
	const char* const AVP_ERR_NULL_PROTO = "NULL_PROTO";
	const char* const AVP_ERR_FILE_NOT_CLOSED = "FILE_NOT_CLOSED";
	const char* const AVP_ERR_SPI_BOUDRATE = "INVALID_SPI_BOUDRATE";
	const char* const AVP_ERR_ACTION = "INVALID_ACTION";

	const char* const AVP_ERR_INVALID_FTYPE = "INVALID_FTYPE";
    
	// SD CARD ERROR
    const char* const AVP_SD_ERRORS[] = {
        "DISK_ERR",
        "INT_ERR",
        "NOT_READY",
        "NO_FILE",
        "NO_PATH",
        "INVALID_NAME",
        "DENIED",
        "EXIST",
        "INVALID_OBJECT",
        "WRITE_PROTECTED",
        "INVALID_DRIVE",
        "NOT_ENABLED",
        "NO_FILESYSTEM",
        "MKFS_ABORTED",
        "TIMEOUT",
        "LOCKED",
        "NOT_ENOUGH_CORE",
        "TOO_MANY_OPEN_FILES",
        "INVALID_PARAMETER"
    };
    const char* const AVP_ERR_SD_PARTIAL_WRITE = "PARTIAL_WRITE";


	// PROGRAMMING ERROR
	const char* const AVP_ERR_ENTER_PMODE = "ENTER_PMODE";
	const char* const AVP_ERR_CHECK_SIG = "CHECK_SIG\nREAD SIG: 0x%02X 0x%02X 0x%02X";
    const char* const AVP_ERR_VERIFY = "VERIFY IN 0x%04X\nEXP: 0x%02X GOT: 0x%02X";
