#include "Global_State.h"
#include "HW_SSD1306.h"
#include "GUI_elements.h"
#include "Ingame_Mechanics.h"
#include "Friend_Meet.h"
#include <math.h>
#include <SPI.h>
#include <Wire.h>

int16_t screen = 6;


String settings_arr[4] = {" 1. Sync",
                      " 2. Name reset",
                      " 3. Age reset",
                      " 4. Weight reset"};
String sync_arr[2] = {" 1. App",
                      " 2. Water Bottle"};

ScreenWrapper settings_list_1("Settings", settings_arr, 4);
ScreenWrapper sync_list_2("Sync", sync_arr, 2);
ScreenWrapper name_reset_3("Name", KEY_ENG, 10, "<-");
ScreenWrapper age_reset_4("Age", KEY_NUM, 2, "<-");
ScreenWrapper weight_reset_5("Weight", KEY_NUM, 2, "<-");
ScreenWrapper name_init_6("Name", KEY_ENG, 10, "->");
ScreenWrapper age_init_7("Age", KEY_NUM, 2, "->");
ScreenWrapper weight_init_8("Weight", KEY_NUM, 2, "->");
ScreenWrapper empty = ScreenWrapper();

ScreenWrapper* getScreen(){
  switch(screen){
    // init screen
    case 6: return &name_init_6;
    case 7: return &age_init_7;
    case 8: return &weight_init_8;

    // main screen
    case 1: return &settings_list_1;
    case 2: return &sync_list_2;
    case 3: return &name_reset_3;
    case 4: return &age_reset_4;
    case 5: return &weight_reset_5;
    default: return &empty;
  }
}

void showScreen(){
  delay(100);
  switch(screen){
    case 0:   duckDisplay_0();      break;
    case 9:   syncApp_9_10();
              screenSwitchMap(0);
              showScreen();         break;
    case 11:  syncBottle_11();      break;
    case 12:  friendMeet_12();      break;
    case 13:  friendMeet_13_14(0);
              screenSwitchMap(0);
              showScreen();         break;
    case 14:  friendMeet_13_14(1);
              screenSwitchMap(0);
              showScreen();         break;
    case -1:  blankScreen();        break;
    default:  getScreen()->draw();  break;
  }
}

void screenSwitchHook(){
  switch(screen){
    case 3: case 6: if(getScreen()->not_empty())    user_name = getScreen()->flush(); break;
    case 4: case 7: if(getScreen()->not_empty())     user_age = getScreen()->flush().toInt(); break;
    case 5: case 8: if(getScreen()->not_empty())  user_weight = getScreen()->flush().toInt(); break;
    case -1: startupDisplay();
    default: break;
  }
}


long long smap[] = {1201, 5040302, 11109, 1, 1, 1, 7, 8, 0, 10, 2, 2, 141300, 12, 12};
void screenSwitchMap(int next){
  screenSwitchHook();
  screen = (int)(smap[screen]%((long long)ceil(pow(100, next+1)))/((long long)ceil(pow(100, next))));
}

void actionMap(){
  /* Actions that need to happen after a button click or game state transition*/ 
  if(fall_edge(3)){
    switch(screen){
      case 0:   growth=(growth+1)%3;                      break; // delete this later
      case 12:  screenSwitchMap(1);                       break;
      default:  getScreen()->moveBackward();              break;
    }showScreen();
  }else if(fall_edge(2)){
    switch(screen){
      case 0:   screenSwitchMap(1);                       break;
      case 12:  screenSwitchMap(2);                       break;
      default:  getScreen()->moveForward();               break;
    }showScreen();
  }else if(fall_edge(1)){
    switch(screen){
      case 0:   screenSwitchMap(0);                       break;
      case 1:   screenSwitchMap(getScreen()->getCurr());  break;
      case 2:   screenSwitchMap(getScreen()->getCurr());  break;
      default:  getScreen()->enter();                     break;
    }showScreen();
  }else if(fall_edge(0)){
    switch(screen){
      case 0: case 9: case 13: case 14:                   break;
      case 1:   screenSwitchMap(4);                       break;
      case 2:   screenSwitchMap(2);                       break;
      default:  screenSwitchMap(0);                       break;
    }showScreen();
  }else if(change_event()) showScreen();
}

void setup(){
  Serial.begin(9600);

  // Step counter setup
//  LSM9DS1_setup();
//  calibrate();

  // IR setup
  Irsetup();

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
