#include <GyverButton.h>
#include <GyverOLED.h>
#include <SPI.h>

#define PODXVAT A0

#define POWER_KEY A2
#define BTN_LEFT 2
#define BTN_OK A3

#define SETTINGS_COUNT 5

typedef struct {
  int spi_speed;
  int sleep_delay;
}params_t;



params_t params;

typedef struct  {
  char pageName[10];
  int *value;
  int min;
  int max;
}setting_t;


setting_t settings[SETTINGS_COUNT]={
  {"SPI speed",&params.spi_speed,1,10},
  {"SleepDelay",&params.sleep_delay,1,100},
};

GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
GButton lBtn(BTN_LEFT);
GButton OkBtn(BTN_OK);
GButton rBtn(POWER_KEY,LOW_PULL);
void Settings(){
  static uint8_t ptr;
  drawMenu(ptr);
}
void drawMenu(uint8_t& page ){
  //oled.clear();
  oled.setScale(2);
  oled.setCursor(10,2);
  oled.print(settings[page].pageName);
  oled.setCursor(10, 3);
  oled.fastLineH(3,10,100);
}
void btnTick(){
  rBtn.tick();
  OkBtn.tick();
  lBtn.tick();
}
void setup() {
  pinMode(A2,INPUT);
  //digitalWrite(PODXVAT,HIGH);
  Serial.begin(9600);
  oled.init();        
  oled.clear();
         
}

void loop() {
  btnTick();
  //Settings();
  if(lBtn.isClick()){
    oled.clear();
    oled.setCursor(0,0);
    oled.print("lBTN");
    }
    if(rBtn.isClick()){
    oled.clear();
    oled.setCursor(0,0);
    oled.print("rBTN");
    }
    if(OkBtn.isClick()){
    oled.clear();
    oled.setCursor(0,0);
    oled.print("OkBTN");
    }
    
}
