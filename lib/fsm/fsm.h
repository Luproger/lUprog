///*
// * fsm.h
// *
// *  Created on: 26 Jan 2026
// *      Author: Luproger
// */
//
//#ifndef FSM_FSM_H_
//#define FSM_FSM_H_
//
//#define SET_MENU_STATE(new, curState, first) setMenuState(new, &curState, &first, __LINE__)
//#define SET_DEV_STATE(new, curState, first) setDevState(new, &curState, &first, __LINE__)
//#define SET_DEV_MODE(new, curState, first) setDevMode(new, &curState, &first, __LINE__)
//#define SET_ERR_STATE(new, curState, first) setErrorState(new, &curState, &first, __LINE__)
//
//typedef enum {
//	MENU_SEL_MOD,
//	MENU_SEL_MCU,
//	MENU_SEL_FIRM,
//	MENU_SEL_CFG,
//	MENU_SEL_EEPR,
//	MENU_PROGRESS,
//}menu_t;
//
//typedef enum {
//	DV_INIT,
//	DV_MENU,
//	DV_SLEEP,
//	DV_ERROR
//}dev_state_t;
//
//typedef enum {
//	DV_USB_SD,
//	DV_FIRMWARE,
//	DV_FUSE,
//	DV_CFG,
//	DV_EEPROM
//}dev_mode_t;
//
//typedef enum {
//	// SD
//	SD_INIT_ERROR,
//	SD_SYSTEM_ERROR,
//
//}error_t;
//
//const char *menu_t_names[] = {
//	"MENU_SEL_MOD",
//	"MENU_SEL_MCU",
//	"MENU_SEL_FIRM",
//	"MENU_SEL_CFG",
//	"MENU_SEL_EEPR",
//	"MENU_PROGRESS"
//};
//
//const char* dev_state_t_names[] = {
//    "DV_INIT",
//    "DV_MENU",
//    "DV_SLEEP",
//    "DV_ERROR"
//};
//
//const char* dev_mode_t_names[] = {
//    "DV_USB_SD",
//    "DV_FIRMWARE",
//    "DV_FUSE",
//    "DV_CFG",
//    "DV_EEPROM"
//};
//
//const char* error_t_names[] = {
//    "SD_INIT_ERROR",
//    "SD_SYSTEM_ERROR"
//};
//
//void setMenuState(menu_t newState, menu_t *curState, boolean *firstInState, uint16_t line);
//void setDevState(dev_state_t newState, dev_state_t *curState, boolean *firstInState, uint16_t line);
//void setDevMode(dev_mode_t newState, dev_mode_t *curState, boolean *firstInState, uint16_t line);
//void setErrorState(dev_mode_t newState, dev_mode_t *curState, boolean *firstInState, uint16_t line);
//
//#endif /* FSM_FSM_H_ */
