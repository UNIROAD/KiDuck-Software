#include "Global_State.h"
#include "HW_SSD1306.h"
#include "GUI_elements.h"
#include "Ingame_Mechanics.h"
#include <math.h>
#include <SPI.h>
#include <Wire.h>

int16_t screen = 7;


String settings_arr[6] = {" 1. Alarm",
                      " 2. Audio",
                      " 3. Name reset",
                      " 4. Age reset",
                      " 5. Weight reset",
                      " 6. Comms"};
List settings_list_1 = listConstruct(settings_arr, 6);

String alarm_arr[4] = {" 1. Feed alarm",
                      " 2. Growth alarm",
                      " 3. Sound alarm",
                      " 4. Sleep alarm"};
List alarm_list_2 = listConstruct(alarm_arr, 4);

String audio_arr[6] = {" 1. Sound on",
                      " 2. Volume",
                      " 3. Music",
                      " 4. Game sound",
                      " 5. More sound",
                      " 6. Main sound"};
List audio_list_3 = listConstruct(audio_arr, 6);

Keyboard name_reset_4 = keyboardConstruct(KEY_ENG);
Textbox name_text_4 = textboxConstruct();
Keyboard age_reset_5 = keyboardConstruct(KEY_NUM);
Textbox age_text_5 = textboxConstruct();
Keyboard weight_reset_9 = keyboardConstruct(KEY_NUM);
Textbox weight_text_9 = textboxConstruct();

String comms_arr[5] = {" 1. Wifi",
                       " 2. Bluetooth"};
List comms_list_6 = listConstruct(comms_arr, 2);

Keyboard name_init_7 = keyboardConstruct(KEY_ENG);
Textbox name_text_7 = textboxConstruct();
Keyboard age_init_8 = keyboardConstruct(KEY_NUM);
Textbox age_text_8 = textboxConstruct();
Keyboard weight_init_10 = keyboardConstruct(KEY_NUM);
Textbox weight_text_10 = textboxConstruct();




void showScreen(){
  delay(100);
  switch(screen){
    // init screen
    case 7: keyboardTextboxDisplay(&name_init_7, &name_text_7, "Name");
            navigationBarDisplay("<", ">", "o", "->"); break;
    case 8: keyboardTextboxDisplay(&age_init_8, &age_text_8, "Age"); 
            navigationBarDisplay("<", ">", "o", "->"); break;
    case 10: keyboardTextboxDisplay(&weight_init_10, &weight_text_10, "Weight"); 
            navigationBarDisplay("<", ">", "o", "->"); break;

    // main screen
    case 0: duckDisplay(); break;
    case 1: listDisplay(&settings_list_1, "Settings"); break;
    case 2: listDisplay(&alarm_list_2, "Alarms"); break;
    case 3: listDisplay(&audio_list_3, "Audio"); break;
    case 4: keyboardTextboxDisplay(&name_reset_4, &name_text_4, "Name"); 
            navigationBarDisplay("<", ">", "o", "<-"); break;
    case 5: keyboardTextboxDisplay(&age_reset_5, &age_text_5, "Age"); 
            navigationBarDisplay("<", ">", "o", "<-"); break;
    case 9: keyboardTextboxDisplay(&weight_reset_9, &weight_text_9, "Weight"); 
            navigationBarDisplay("<", ">", "o", "<-"); break;
    case 6: listDisplay(&comms_list_6, "Communications"); break;
    
    // reserved state
    case -1: blankScreen(); break;
    default: break;
  }
}

void screenSwitchHook(){
  switch(screen){
    case 4: if(name_text_4.getText().length()) user_name = name_text_4.flush(); break;
    case 5: if(age_text_5.getText().length()) user_age = age_text_5.flush().toInt(); break;
    case 9: if(weight_text_9.getText().length()) user_weight = weight_text_9.flush().toInt(); break;
    case 7: if(name_text_7.getText().length()) user_name = name_text_7.flush(); break;
    case 8: if(age_text_8.getText().length()) user_age = age_text_8.flush().toInt(); break;
    case 10: if(weight_text_10.getText().length()) user_weight = weight_text_10.flush().toInt(); break;
    case -1: startupDisplay();
    default: break;
  }
}


long long smap[] = {1, 60905040302, 1, 1, 1, 1, 1, 8, 10, 1, 0};
void screenSwitchMap(int next){
  screenSwitchHook();
  screen = (int)(smap[screen]%((long long)ceil(pow(100, next+1)))/((long long)ceil(pow(100, next))));
}

void actionMap(){
  if(fall_edge(3)){
    switch(screen){
            case 0: growth=(growth+1)%3; break; // delete this later
      case 1: settings_list_1.moveBackward(); break;
      case 2: alarm_list_2.moveBackward(); break;
      case 3: audio_list_3.moveBackward(); break;
      case 4: name_reset_4.moveBackward(); break;
      case 5: age_reset_5.moveBackward(); break;
      case 6: comms_list_6.moveBackward(); break;
      case 7: name_init_7.moveBackward(); break;
      case 8: age_init_8.moveBackward(); break;
      case 9: weight_reset_9.moveBackward(); break;
      case 10: weight_init_10.moveBackward(); break;
      default: break;
    }showScreen();
  }else if(fall_edge(2)){
    switch(screen){
      case 1: settings_list_1.moveForward(); break;
      case 2: alarm_list_2.moveForward(); break;
      case 3: audio_list_3.moveForward(); break;
      case 4: name_reset_4.moveForward(); break;
      case 5: age_reset_5.moveForward(); break;
      case 6: comms_list_6.moveForward(); break;
      case 7: name_init_7.moveForward(); break;
      case 8: age_init_8.moveForward(); break;
      case 9: weight_reset_9.moveForward(); break;
      case 10: weight_init_10.moveForward(); break;
      default: break;
    }showScreen();
  }else if(fall_edge(1)){
    switch(screen){
      case 0: screenSwitchMap(0); break;
      case 1: screenSwitchMap(settings_list_1.getCurr()); break;
      case 4: name_reset_4.getCurr(&name_text_4); break;
      case 5: age_reset_5.getCurr(&age_text_5); break;
      case 7: name_init_7.getCurr(&name_text_7); break;
      case 8: age_init_8.getCurr(&age_text_8); break;
      case 9: weight_reset_9.getCurr(&weight_text_9); break;
      case 10: weight_init_10.getCurr(&weight_text_10); break;
      default: break;
    }showScreen();
  }else if(fall_edge(0)){
    switch(screen){
      case 1: screenSwitchMap(6); break;
      case 2: case 3: case 4: case 5: case 6: case 7: 
      case 8: case 9: case 10: screenSwitchMap(0); break;
      default: break;
    }showScreen();
  }
  else if(change_event()) showScreen();
}

void setup(){
  Serial.begin(9600);

  // Step counter setup
//  LSM9DS1_setup();
//  calibrate();

  // Screen setup
  SSD1306_Setup();
  startupDisplay();
  showScreen();

  // Button setup
  setButtonPinMode();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  // Step counter
//  if(step_clock.elapsed_time_check()) step_count();

  // Screen refresh & Button input process
  actionMap();
}
