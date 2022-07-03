#include "Global_State.h"
#include "HW_SSD1306.h"
//#include "GUI_elements.h"

#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

int16_t screen = 0;

List listtest = listConstructTest();


void setup(){
 Serial.begin(9600);
 SSD1306_Setup();
 
 startupDisplay();
 duckDisplay();

 setButtonPinMode();
 pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  if(screen==1){
    if(buttonMapList(&listtest)){
      delay(1);
      listDisplay(&listtest);
    }
  }
  if(fall_edge(3, &prev3, &curr3)){
    screen=(screen+1)%2;
    if(screen) listDisplay(&listtest);
    else duckDisplay();
  }
}


//////////////////////////////////////////////////////////


// void setup(){
//   Serial.begin(9600);

//   SSD1306_Setup();
//   cleartest();
  
//   setButtonPinMode();
  
//   pinMode(LED_BUILTIN, OUTPUT);
// }

// void loop(){
//   if(buttonPressDetect()) cleartest();
// }

// void cleartest(){
//   for(int i = 0; i < 2; i++){
//     // LED 블링크 0.5초 켜고, 0.5초 끄고
//     digitalWrite(LED_BUILTIN, HIGH);
//     delay(1);
//     digitalWrite(LED_BUILTIN, LOW);
//   }
//   screen = (++screen)%3;
//   rectButtonSetDisplay(screen);
//   navigationBarDisplay("ai", "bi", "ci", "di");
//   delay(100);
// }
