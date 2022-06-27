#include "Global_State.h"
#include "HW_SSD1306.h"

#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

int16_t screen = 2;

void setup(){
  Serial.begin(9600);

  SSD1306_Setup();
  cleartest();
  
  setButtonPinMode();
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  if(buttonPressDetect()) cleartest();
}

void cleartest(){
  for(int i = 0; i < 2; i++){
    // LED 블링크 0.5초 켜고, 0.5초 끄고
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1);
    digitalWrite(LED_BUILTIN, LOW);
  }
  screen = (++screen)%3;
  switch(screen){
    case 0:
      testrect();
      break;
    case 1:
      interruptButton();
      break;
    case 2:
      interruptButton2();
      break;
    default:
      testrect();
  }
  delay(100);
}
