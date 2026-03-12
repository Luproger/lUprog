///*
// * fsm.c
// *
// *  Created on: 26 Jan 2026
// *      Author: Luproger
// */
//#include "main.h"
//
//void setMenuState(menu_t newState, menu_t *curState, boolean *firstInState, uint16_t line) {
//	DEBUG_PRINTF("Set new MENU state on line %d!\n\r", line);
//	DEBUG_PRINTF("  Old state: %s\n\r", menu_t_names[*curState]);
//	DEBUG_PRINTF("  New state: %s\n\r", menu_t_names[newState]);
//
//	*curState = newState;
//
//	if(firstInState){
//		*firstInState = 1;
//	}
//}
//
//void setDevState(dev_state_t newState, dev_state_t *curState, boolean *firstInState, uint16_t line) {
//	DEBUG_PRINTF("Set new DEV_STATE on line %d!\n\r", line);
//	DEBUG_PRINTF("  Old state: %s\n\r", dev_state_t_names[*curState]);
//	DEBUG_PRINTF("  New state: %s\n\r", dev_state_t_names[newState]);
//
//	*curState = newState;
//
//	if(firstInState){
//		*firstInState = 1;
//	}
//}
//
//void setDevMode(dev_mode_t newState, dev_mode_t *curState, boolean *firstInState, uint16_t line) {
//	DEBUG_PRINTF("Set new DEV_MODE on line %d!\n\r", line);
//	DEBUG_PRINTF("  Old state: %s\n\r", dev_mode_t_names[*curState]);
//	DEBUG_PRINTF("  New state: %s\n\r", dev_mode_t_names[newState]);
//
//	*curState = newState;
//
//	if(firstInState){
//		*firstInState = 1;
//	}
//}
//
//void setErrorState(dev_mode_t newState, dev_mode_t *curState, boolean *firstInState, uint16_t line) {
//	DEBUG_PRINTF("Set new ERR_STATE on line %d!\n\r", line);
//	DEBUG_PRINTF("  Old state: %s\n\r", error_t_names[*curState]);
//	DEBUG_PRINTF("  New state: %s\n\r", error_t_names[newState]);
//
//	*curState = newState;
//
//	if(firstInState){
//		*firstInState = 1;
//	}
//}
