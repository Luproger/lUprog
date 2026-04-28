/*
 * error_list.h
 *
 *  Created on: 24 Apr 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_ERROR_LIST_H_
#define AVR_PROGRAMMER_ERROR_LIST_H_


	// INIT AVR_PROGRAMMER
	extern const char* const AVP_ERR_INIT_CONF;
	extern const char* const AVP_ERR_MISSING_INIT;

	// INIT SESSION
	extern const char* const AVP_ERR_NULL_MCU;
	extern const char* const AVP_ERR_NULL_PATH;
	extern const char* const AVP_ERR_NULL_PROTO;
	extern const char* const AVP_ERR_FILE_NOT_CLOSED;
	extern const char* const AVP_ERR_FILE_FORMAT;
	extern const char* const AVP_ERR_SPI_BOUDRATE;

	extern const char* const AVP_ERR_ACTION;

	// SD CARD ERROR
	extern const char* const AVP_SD_ERRORS[];


	// PROGRAMMING ERROR

	extern const char* const AVP_ERR_ENTER_PMODE;
	extern const char* const AVP_ERR_CHECK_SIG;


#endif /* AVR_PROGRAMMER_ERROR_LIST_H_ */
