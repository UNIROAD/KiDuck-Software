#include "Global_State.h"
#include "HW_SSD1306.h"
//#include "GUI_elements.h"
#include <math.h>

#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

int16_t screen = 0;

string temptemp[5] = {" 1. Alarm",
                      " 2. Audio",
                      " 3. Name reset",
                      " 4. Age reset",
                      " 5. Comms"};
List listtest_1 = listConstruct(temptemp, 5);

char keytemp[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
Keyboard keytest_2 = keyboardConstruct(keytemp, 27);

void showScreen(){
  delay(1);
  switch(screen){
    case 0: duckDisplay_0(); break;
    case 1: listDisplay(&listtest_1, "Settings"); break;
    case 2: keyboardDisplay(&keytest_2); break;
    default: break;
  }
}

int smap[] = {1, 2, 0};
void screenSwitchMap(int next){screen = smap[screen]%((int)pow(10, next+1))/((int)pow(10, next));}

void actionMap(){
  if(fall_edge(3)){
    switch(screen){
      case 1: listtest_1.moveBackward(); break;
      case 2: keytest_2.moveBackward(); break;
      default: break;
    }
  }else if(fall_edge(2)){
    switch(screen){
      case 1: listtest_1.moveForward(); break;
      case 2: keytest_2.moveForward(); break;
      default: break;
    }
  }else if(fall_edge(1)){
    screenSwitchMap(0);
  }else if(fall_edge(0)){
  }
  showScreen();
}

void setup(){
 Serial.begin(9600);
 SSD1306_Setup();
 
 startupDisplay();
 showScreen();

 setButtonPinMode();
 pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  actionMap();
}
