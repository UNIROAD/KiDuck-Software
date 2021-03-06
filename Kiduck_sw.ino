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


string settings_arr[5] = {" 1. Alarm",
                      " 2. Audio",
                      " 3. Name reset",
                      " 4. Age reset",
                      " 5. Comms"};
List settings_list_1 = listConstruct(settings_arr, 5);

string alarm_arr[4] = {" 1. Feed alarm",
                      " 2. Growth alarm",
                      " 3. Sound alarm",
                      " 4. Sleep alarm"};
List alarm_list_2 = listConstruct(alarm_arr, 4);

string audio_arr[6] = {" 1. Sound on",
                      " 2. Volume",
                      " 3. Music",
                      " 4. Game sound",
                      " 5. More sound",
                      " 6. Main sound"};
List audio_list_3 = listConstruct(audio_arr, 6);

Keyboard name_reset_4 = engKeyboardConstruct();
Textbox name_text_4 = textboxConstruct();
Keyboard age_reset_5 = engKeyboardConstruct();
Textbox age_text_5 = textboxConstruct();

string comms_arr[5] = {" 1. Wifi",
                       " 2. Bluetooth"};
List comms_list_6 = listConstruct(comms_arr, 2);




void showScreen(){
  delay(1);
  switch(screen){
    case 0: duckDisplay_0(); break;
    case 1: listDisplay(&settings_list_1, "Settings"); break;
    case 2: listDisplay(&alarm_list_2, "Alarms"); break;
    case 3: listDisplay(&audio_list_3, "Audio"); break;
    case 4: keyboardTextboxDisplay(&name_reset_4, &name_text_4, "Name"); break;
    case 5: keyboardTextboxDisplay(&age_reset_5, &age_text_5, "Age"); break;
    case 6: listDisplay(&comms_list_6, "Communications"); break;
    default: break;
  }
}

int smap[] = {1, 65432, 1, 1, 1, 1, 1};
void screenSwitchMap(int next){screen = smap[screen]%((int)pow(10, next+1))/((int)pow(10, next));}

void actionMap(){
  if(fall_edge(3)){
    switch(screen){
      case 1: settings_list_1.moveBackward(); break;
      case 2: alarm_list_2.moveBackward(); break;
      case 3: audio_list_3.moveBackward(); break;
      case 4: name_reset_4.moveBackward(); break;
      case 5: age_reset_5.moveBackward(); break;
      case 6: comms_list_6.moveBackward(); break;
      default: break;
    }
  }else if(fall_edge(2)){
    switch(screen){
      case 1: settings_list_1.moveForward(); break;
      case 2: alarm_list_2.moveForward(); break;
      case 3: audio_list_3.moveForward(); break;
      case 4: name_reset_4.moveForward(); break;
      case 5: age_reset_5.moveForward(); break;
      case 6: comms_list_6.moveForward(); break;
      default: break;
    }
  }else if(fall_edge(1)){
    switch(screen){
      case 0: screenSwitchMap(0); break;
      case 1: screenSwitchMap(settings_list_1.getCurr()); break;
      case 4: name_reset_4.enter(&name_text_4); break;
      case 5: age_reset_5.enter(&age_text_5); break;
      default: break;
    }
  }else if(fall_edge(0)){
    switch(screen){
      case 1: screenSwitchMap(5); break;
      case 2: case 3: case 4: case 5: 
      case 6: screenSwitchMap(0); break;
      default: break;
    }
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
