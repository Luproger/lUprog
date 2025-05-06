#include <GyverButton.h>
#include <GyverOLED.h>
#include <SPI.h>

#define PODXVAT A0

#define POWER_KEY A2
#define BTN_LEFT 2
#define BTN_OK A3

#define SETTINGS_COUNT 5

typedef enum {
  MAIN_SCREEN,
  SETTINGS,
  SET_MK,
  BOOT_TO_CD,
  FUSE,
  FLASH,
  EEPROM_SETUP,
  BOOTLOADER,
  READ,
  WRITE,
  ARD_BOOT,
  STD_BOOT,
} menu_t;

char *stateName[] = {
  "lUprog",
  "SETTINGS",
  "SET_MK",
  "BOOT_TO_CD",
  "FUSE",
  "FLASH",
  "EEPROM",
  "BOOTLOADER",
  "READ",
  "WRITE",
  "ARD_BOOT",
  "STD_BOOT",
};


menu_t Menu = MAIN_SCREEN;

typedef struct {
  int spi_speed;
  int sleep_delay;
} params_t;



params_t params;

typedef struct  {
  char pageName[10];
  int *value;
  int min;
  int max;
} setting_t;


setting_t settings[SETTINGS_COUNT] = {
  {"SPI speed", &params.spi_speed, 1, 10},
  {"SleepDelay", &params.sleep_delay, 1, 100},
};

GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
GButton lBtn(BTN_LEFT);
GButton OkBtn(BTN_OK);
GButton rBtn(POWER_KEY, LOW_PULL);

void Settings() {
  static uint8_t ptr;
  //drawMenu(ptr);
}
/*void drawMenu(uint8_t& page ) {
  //oled.clear();
  oled.setScale(2);
  oled.setCursor(10, 2);
  oled.print(settings[page].pageName);
  oled.setCursor(10, 3);
  oled.fastLineH(3, 10, 100);
  }*/
void btnTick() {
  rBtn.tick();
  OkBtn.tick();
  lBtn.tick();
}
void setup() {
  pinMode(A2, INPUT);
  //digitalWrite(PODXVAT,HIGH);
  Serial.begin(9600);
  oled.init();
  oled.clear();

}

bool isFirst = true;

void changeSt(menu_t newSt, int line) {
  Serial.print("Переход в новое состояние ");
  Serial.print(stateName[newSt]);
  Serial.print(", на строке ");
  Serial.println(line);
  isFirst = true;
  Menu = newSt;
}
void drawMenuPtr() {
  static uint16_t blinkTimer;
  static bool blinkF;
  if (millis() - blinkTimer >= 500) {
    if (blinkF) oled.clear(0, 29, 132, 31);
    else oled.fastLineH(30, 0, 132);
    blinkF = !blinkF;
    blinkTimer = millis();
  }
}

void drawMenu() {
  if (isFirst) {
    oled.clear();
    oled.setScale(2);
    oled.setCursor(0, 1);
    oled.print(stateName[Menu]);
    isFirst = false;

  }
}

void loop() {
  btnTick();
  drawMenu();
  drawMenuPtr();
  switch (Menu) {
    case MAIN_SCREEN:
      if (OkBtn.isClick()) {
        changeSt(SETTINGS, __LINE__);
      }
      break;
    case SETTINGS:
      if (OkBtn.isHold()) {
        changeSt(MAIN_SCREEN, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(SETTINGS, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( SET_MK, __LINE__);
      }
      break;
    case SET_MK:
      if (lBtn.isClick()) {
        changeSt(SETTINGS, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(BOOT_TO_CD, __LINE__);
      }
      break;
    case BOOT_TO_CD:
      if (OkBtn.isHold()) {
        changeSt(SET_MK, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(BOOT_TO_CD, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( FUSE, __LINE__);
      }
      break;
    case FUSE:
      if (OkBtn.isHold()) {
        changeSt(SET_MK, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt(BOOT_TO_CD, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(READ, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( FLASH, __LINE__);
      }
      break;
    case FLASH:
      if (OkBtn.isHold()) {
        changeSt(SET_MK, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt(FUSE, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(READ, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( EEPROM_SETUP, __LINE__);
      }
      break;
    case EEPROM_SETUP:
      if (OkBtn.isHold()) {
        changeSt(SET_MK, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt(FLASH, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(READ, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( BOOTLOADER, __LINE__);
      }
      break;
    case BOOTLOADER:
      if (OkBtn.isHold()) {
        changeSt(SET_MK, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt(EEPROM_SETUP, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(ARD_BOOT, __LINE__);
      }
      break;
    case READ:
      if (OkBtn.isHold()) {
        changeSt(BOOT_TO_CD, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(MAIN_SCREEN, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( WRITE, __LINE__);
      }
      break;
    case WRITE:
      if (OkBtn.isHold()) {
        changeSt(BOOT_TO_CD, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(MAIN_SCREEN, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt( READ, __LINE__);
      }
      break;
    case ARD_BOOT:
      if (OkBtn.isHold()) {
        changeSt(BOOTLOADER, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(MAIN_SCREEN, __LINE__);
      }
      if (rBtn.isClick()) {
        changeSt( STD_BOOT, __LINE__);
      }
      break;
    case STD_BOOT:
      if (OkBtn.isHold()) {
        changeSt(BOOTLOADER, __LINE__);
      }
      if (OkBtn.isClick()) {
        changeSt(MAIN_SCREEN, __LINE__);
      }
      if (lBtn.isClick()) {
        changeSt( ARD_BOOT, __LINE__);
      }
      break;
  }
}
