#include "Global_State.h"
#include "HW_SSD1306.h"
//#include "GUI_elements.h"

#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

int16_t screen = 2;


/*
* 
* 
* 
* 
*/

List listtest = listConstructTest();

// function that displays List screen
void listDisplay(){
  Div div = Div(listtest.getWidth(), listtest.getHeight(), 
                listtest.getXPos(), listtest.getYPos(), 
                1, listtest.getVisibleLen(), 2);
  string vis_text;

  display.clearDisplay();

  display.fillRect(div.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                  div.position(listtest.getCursorPos(), DIV_HEIGHT_DIRECTION, DIV_PADDED),
                  div.getSectWidth(),
                  div.getSectHeight(),
                  SSD1306_WHITE);

  display.setTextSize(1); // Draw 1:1-scale text

  for(int i=0;i<listtest.getVisibleLen();i++){
    vis_text = listtest.getVisibleText(i);

    display.setTextColor((i==listtest.getCursorPos())?SSD1306_BLACK:SSD1306_WHITE);
    display.setCursor(div.textAllign(vis_text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, 0, DIV_LEFT_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(TEXT_HEIGHT, i, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F(vis_text.c_str()));
    delay(1);
  }

  // Title
  display.drawLine(0, div.getSectHeight()+2, div.getSectWidth()+2, div.getSectHeight()+2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(F(" Settings"));

  display.display();
  delay(1);
}




bool buttonMapList(){
 if(fall_edge(5, &prev5, &curr5)){
   listtest.moveBackward();
   return true;
 }
 else if(fall_edge(4, &prev4, &curr4)){
   listtest.moveForward();
   return true;
 }
 return false;
}



/*
* 
* 
* 
* 
*/

void setup(){
 Serial.begin(9600);

 SSD1306_Setup();

 listDisplay();

 navigationBarDisplay("^", "v", "=", "o");

 setButtonPinMode();
 
 pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  if(buttonMapList()){
    Serial.println(listtest.getCursorPos());
    delay(1);

    listDisplay();

    navigationBarDisplay("^", "v", "=", "o");
  }
}



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
