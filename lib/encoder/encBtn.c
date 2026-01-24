/*
 * encBtn.c
 *
 *  Created on: 13 Jan 2026
 *      Author: Luproger
 */
#include "encBtn.h"


// ----------------- КНОПКА -----------------

void init_btn(button_t *btn,GPIO_TypeDef *potr,uint32_t pin,bool pull_up, bool invser) {


	if (pin != BTN_NO_PIN) {
        btn->_PIN = pin;
        btn->_PORT = potr;
        btn->flags.noPin = false;
    } else {
        btn->flags.noPin = true;
    }

    btnSetType(btn,pull_up);
    btn->flags.mode = false;
    btn->flags.tickMode = false;
    btn->flags.inv_state = invser;


    btn->_debounce = 60;
    btn->_timeout = 500;
    btn->_click_timeout = 500;
    btn->_step_timeout = 400;
    btn->btn_counter = 0;
	btn->last_counter = 0;
	btn->last_hold_counter = 0;
	btn->btn_timer = 0;
	btn->btn_state = false;
	btn->btn_flag = false;
}

// ==================== SET ====================
void btnSetDebounce(button_t *btn, uint16_t debounce) {
    btn->_debounce = debounce;
}
void btnSetTimeout(button_t *btn, uint16_t new_timeout) {
    btn->_timeout = new_timeout;
}
void btnSetClickTimeout(button_t *btn, uint16_t new_timeout) {
    btn->_click_timeout = new_timeout;
}
void btnSetStepTimeout(button_t *btn, uint16_t step_timeout) {
    btn->_step_timeout = step_timeout;
}


void btnSetType(button_t *btn, bool type) {
	#warning "UNSUPPORT, US CUBE CONFIG"

	btn->flags.type = type;
	/*
    if (!btn->flags.noPin) {
        if (type) pinMode(_PIN, INPUT);
        else pinMode(_PIN, INPUT_PULLUP);
    }
    */
}


void btnSetDirection(button_t *btn, bool dir) {
    btn->flags.inv_state = dir;
}
void btnSetTickMode(button_t *btn, bool tickMode) {
    btn->flags.tickMode = tickMode;
}

// ==================== IS ====================
boolean btnIsPress(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.isPress_f) {
        btn->flags.isPress_f = false;
        return true;
    } else return false;
}
boolean btnIsRelease(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.isRelease_f) {
        btn->flags.isRelease_f = false;
        return true;
    } else return false;
}
boolean btnIsClick(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.isOne_f) {
        btn->flags.isOne_f = false;
        return true;
    } else return false;
}
boolean btnIsHolded(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.isHolded_f) {
        btn->flags.isHolded_f = false;
        return true;
    } else return false;
}
boolean btnIsHold(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.step_flag) return true;
    else return false;
}
boolean btnState(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    return btn->btn_state;
}
boolean btnIsSingle(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.counter_flag && btn->last_counter == 1) {
        //        last_counter = 0;
        //        flags.counter_flag = false;
        btn->flags.counter_reset = true;
        return true;
    } else return false;
}
boolean btnIsDouble(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.counter_flag && btn->last_counter == 2) {
        //        flags.counter_flag = false;
        //        last_counter = 0;
        btn->flags.counter_reset = true;
        return true;
    } else return false;
}
boolean btnIsTriple(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.counter_flag && btn->last_counter == 3) {
        //        flags.counter_flag = false;
        //        last_counter = 0;
        btn->flags.counter_reset = true;
        return true;
    } else return false;
}
boolean btnHasClicks(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    if (btn->flags.counter_flag) {
        //        flags.counter_flag = false;
        btn->flags.counter_reset = true;
        return true;
    } else return false;
}
uint8_t btnGetClicks(button_t *btn) {
    //    byte thisCount = last_counter;            // Исключено    14.01.2021
    //        last_counter = 0;
    btn->flags.counter_reset = true;
    return btn->last_counter;                    //    return thisCount;    (замена)    14.01.2021
}
uint8_t btnGetHoldClicks(button_t *btn) {
    if (btn->flags.tickMode) btnTick(btn);
    return btn->last_hold_counter;                //return flags.hold_flag ? last_hold_counter : 0;    (Замена)     14.01.2021
}
boolean btnIsStepClick(button_t *btn, byte clicks) {
    if ( btn->flags.tickMode) btnTick(btn);
    if ( btn->btn_counter == clicks &&  btn->flags.step_flag && (_getTickMS() -  btn->btn_timer >= btn->_step_timeout)) {
        btn->btn_timer = _getTickMS();
        return true;
    }
    else return false;
}
boolean btnIsStep(button_t *btn) {
	return btnIsStepClick(btn,0);
}

void btnResetStates(button_t *btn) {
    btn->flags.isPress_f = false;
    btn->flags.isRelease_f = false;
    btn->flags.isOne_f = false;
    btn->flags.isHolded_f = false;
    btn->flags.step_flag = false;
    btn->flags.counter_flag = false;
    btn->last_hold_counter = 0;
    btn->last_counter = 0;
}

// ==================== TICK ====================
void btnTick_state(button_t *btn, boolean state) {
    btn->flags.mode = true;
    btn->btn_state = state ^ btn->flags.inv_state;
    btnTick(btn);
    btn->flags.mode = false;
}

void btnTick(button_t *btn) {
    // читаем пин
    if (!btn->flags.mode && !btn->flags.noPin) btn->btn_state = !_buttonRead() ^ (btn->flags.inv_state ^ btn->flags.type);

    uint32_t thisMls = _getTickMS();

    // нажатие
    if (btn->btn_state && !btn->btn_flag) {
        if (!btn->flags.btn_deb) {
            btn->flags.btn_deb = true;
            btn->btn_timer = thisMls;
        } else {
            if (thisMls - btn->btn_timer >= btn->_debounce) {
                btn->btn_flag = true;
                btn->flags.isPress_f = true;
                btn->flags.oneClick_f = true;
            }
        }
    } else {
        btn->flags.btn_deb = false;
    }

    // отпускание
    if (!btn->btn_state && btn->btn_flag) {
        btn->btn_flag = false;
        if (!btn->flags.hold_flag) btn->btn_counter++;
        btn->flags.hold_flag = false;
        btn->flags.isRelease_f = true;
        btn->btn_timer = thisMls;
        if (btn->flags.step_flag) {
            btn->last_counter = 0;
            btn->btn_counter = 0;
            btn->flags.step_flag = false;
        }
        if (btn->flags.oneClick_f) {
            btn->flags.oneClick_f = false;
            btn->flags.isOne_f = true;
        }
    }

    // кнопка удерживается
    if (btn->btn_flag && btn->btn_state && (thisMls - btn->btn_timer >= btn->_timeout) && !btn->flags.hold_flag) {
        btn->flags.hold_flag = true;
        btn->last_hold_counter = btn->btn_counter;
        //btn_counter = 0;
        //last_counter = 0;
        btn->flags.isHolded_f = true;
        btn->flags.step_flag = true;
        btn->flags.oneClick_f = false;
        btn->btn_timer = thisMls;
    }

    // обработка накликивания
    if ((thisMls - btn->btn_timer >= btn->_click_timeout) && (btn->btn_counter != 0) &&  !btn->btn_state) {    //И здесь еще добавлен !btn_state
        btn->last_counter = btn->btn_counter;
        btn->btn_counter = 0;
        btn->flags.counter_flag = true;
    }

    // сброс накликивания                        //Добавлено
    if (btn->flags.counter_reset) {
        btn->last_counter = 0;
        btn->flags.counter_flag = false;
        btn->flags.counter_reset = false;
    }
}

// ----------------- ЭНКОДЕР -----------------

void init_enc(encoder_t *enc, TIM_HandleTypeDef *htim){
	enc->htim = htim;
	HAL_TIM_Encoder_Start(enc->htim, TIM_CHANNEL_ALL);
	encReset(enc);
}

int8_t encGetState(encoder_t *enc){
	uint16_t currCount = (__HAL_TIM_GET_COUNTER(enc->htim) / 4);
	int8_t currState = (currCount - enc->state);
	enc->state = currCount;
	return currState;
}

void encReset(encoder_t *enc){
	encGetState(enc);
	enc->ccw = 0;
	enc->cw = 0;
}

bool encIsCW(encoder_t *enc){ // Right
	bool t = enc->cw;
	enc->cw=0;
	return t;
}
bool encIsCCW(encoder_t *enc){ // Left
	bool t = enc->ccw;
	enc->ccw=0;
	return t;
}
//bool encIsHoldCW(encoder_t *enc, button_t *btn){
//	if(encIsCW(enc) && btnIsHold(btn)){
//		return true;
//	}
//	return false;
//}
//bool encIsHoldCCW(encoder_t *enc, button_t *btn){
//	if(encIsCCW(enc) && btnIsHold(btn)){
//		return true;
//	}
//	return false;
//}
void encTick(encoder_t *enc){
	int8_t t = encGetState(enc);
	if(t<0 && enc->cw==0){
		enc->ccw = 1;

	}else if(t>0 && enc->ccw==0){
		enc->cw = 1;
	}
}
