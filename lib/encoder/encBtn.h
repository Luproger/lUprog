/*
 * encBtn.h
 *
 *  Created on: 13 Jan 2026
 *      Author: Luproger
 */

#ifndef ENCBTN_H_
#define ENCBTN_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f1xx_hal.h"

// ----------------- КНОПКА -----------------

#define _buttonRead() HAL_GPIO_ReadPin(btn->_PORT,btn->_PIN)

#define _getTickMS() HAL_GetTick()
#define byte uint8_t
#define boolean bool

#define BTN_NO_PIN -1
#define HIGH_PULL 0
#define LOW_PULL 1
#define NORM_OPEN 0
#define NORM_CLOSE 1
#define MANUAL 0
#define AUTO 1

typedef struct {
    bool btn_deb: 1;
    bool hold_flag: 1;
    bool counter_flag: 1;
    bool isHolded_f: 1;
    bool isRelease_f: 1;
    bool isPress_f: 1;
    bool step_flag: 1;
    bool oneClick_f: 1;
    bool isOne_f: 1;
    bool inv_state: 1;
    bool mode: 1;
    bool type: 1;
    bool tickMode: 1;
    bool noPin: 1;
    bool counter_reset: 1;            //Добавлен
} ButtonFlags;

typedef struct
{
    // GButton(int8_t pin = BTN_NO_PIN, bool type = HIGH_PULL, bool dir = NORM_OPEN);    // конструктор кнопки, принимает PIN пин, тип type (HIGH_PULL / LOW_PULL) и направление dir (NORM_OPEN / NORM_CLOSE)
    // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC, pinMode - INPUT_PULLUP (по умолчанию)
    // LOW_PULL - кнопка подключена к VCC, пин подтянут к GND, pinMode - INPUT
    // NORM_OPEN - кнопка по умолчанию разомкнута (по умолчанию)
    // NORM_CLOSE - кнопка по умолчанию замкнута

    //-------------------------------------
    ButtonFlags flags;
    uint32_t _PIN;
    GPIO_TypeDef *_PORT;
    uint16_t _debounce;
    uint16_t _timeout;
    uint16_t _click_timeout ;
    uint16_t _step_timeout;
    uint8_t btn_counter,
    last_counter,
	last_hold_counter;
    uint32_t btn_timer;
    bool btn_state;
    bool btn_flag;
    /* data */
}  button_t;

void init_btn(button_t *btn,GPIO_TypeDef *potr,uint32_t pin,bool pull_up, bool invser);


void btnSetDebounce(button_t *btn,uint16_t debounce);                // установка времени антидребезга (по умолчанию 80 мс)
void btnSetTimeout(button_t *btn,uint16_t new_timeout);              // установка таймаута удержания (по умолчанию 300 мс)
void btnSetClickTimeout(button_t *btn,uint16_t new_timeout);         // установка таймаута между кликами (по умолчанию 500 мс)
void btnSetStepTimeout(button_t *btn,uint16_t step_timeout);         // установка таймаута между инкрементами (по умолчанию 400 мс)
void btnSetType(button_t *btn,bool type);                            // установка типа кнопки (HIGH_PULL - подтянута к питанию, LOW_PULL - к gnd)
void btnSetDirection(button_t *btn,bool dir);                        // установка направления (разомкнута/замкнута по умолчанию - NORM_OPEN, NORM_CLOSE)

void btnSetTickMode(button_t *btn,bool tickMode);                    // (MANUAL / AUTO) ручной или автоматический опрос кнопки функцией tick()
// MANUAL - нужно вызывать функцию tick() вручную
// AUTO - tick() входит во все остальные функции и опрашивается сама

void btnTick(button_t *btn);                      // опрос кнопки
void btnTick_state(button_t *btn, boolean state); // опрос внешнего значения (1 нажато, 0 не нажато) (для матричных, резистивных клавиатур и джойстиков)


/*---------------------*/
boolean btnIsPress(button_t *btn);      // возвращает true при нажатии на кнопку. Сбрасывается после вызова
boolean btnIsRelease(button_t *btn);    // возвращает true при отпускании кнопки. Сбрасывается после вызова
boolean btnIsClick(button_t *btn);      // возвращает true при клике. Сбрасывается после вызова
boolean btnIsHolded(button_t *btn);     // возвращает true при удержании дольше timeout. Сбрасывается после вызова
boolean btnIsHold(button_t *btn);       // возвращает true при нажатой кнопке, не сбрасывается
boolean btnState(button_t *btn);        // возвращает состояние кнопки

boolean btnIsSingle(button_t *btn);        // возвращает true при одиночном клике. Сбрасывается после вызова
boolean btnIsDouble(button_t *btn);        // возвращает true при двойном клике. Сбрасывается после вызова
boolean btnIsTriple(button_t *btn);        // возвращает true при тройном клике. Сбрасывается после вызова

boolean btnHasClicks(button_t *btn);    // проверка на наличие кликов. Сбрасывается после вызова
uint8_t btnGetClicks(button_t *btn);    // вернуть количество кликов
uint8_t btnGetHoldClicks(button_t *btn);// вернуть количество кликов, предшествующее удерживанию

boolean btnIsStep(button_t *btn); // возвращает true по таймеру setStepTimeout, смотри пример
boolean btnIsStepClick(button_t *btn,byte clicks); // возвращает true по таймеру setStepTimeout, смотри пример

void btnResetStates(button_t *btn);        // сбрасывает все is-флаги и счётчики

// ----------------- ЭНКОДЕР -----------------

typedef struct{
	TIM_HandleTypeDef *htim;
	uint16_t state;
	bool cw;
	bool ccw;
}encoder_t;

void init_enc(encoder_t *enc, TIM_HandleTypeDef *htim);
void encTick(encoder_t *enc);

void encReset(encoder_t *enc);
int8_t encGetState(encoder_t *enc);

bool encIsCW(encoder_t *enc);
bool encIsCCW(encoder_t *enc);
//bool encIsHoldCW(encoder_t *enc, button_t *btn);
//bool encIsHoldCCW(encoder_t *enc, button_t *btn);


#endif /* ENCBTN_H_ */
