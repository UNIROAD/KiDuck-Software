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
String alarm_arr[4] = {" 1. Feed alarm",
                      " 2. Growth alarm",
                      " 3. Sound alarm",
                      " 4. Sleep alarm"};
String audio_arr[6] = {" 1. Sound on",
                      " 2. Volume",
                      " 3. Music",
                      " 4. Game sound",
                      " 5. More sound",
                      " 6. Main sound"};
String comms_arr[5] = {" 1. Wifi",
                       " 2. Bluetooth"};

ScreenWrapper settings_list_1("Settings", settings_arr, 6);
ScreenWrapper alarm_list_2("Alarm", alarm_arr, 4);
ScreenWrapper audio_list_3("Audio", audio_arr, 6);
ScreenWrapper name_reset_4("Name", KEY_ENG, "<-");
ScreenWrapper age_reset_5("Age", KEY_NUM, "<-");
ScreenWrapper comms_list_6("Communications", comms_arr, 2);
ScreenWrapper name_init_7("Name", KEY_ENG, "->");
ScreenWrapper age_init_8("Age", KEY_NUM, "->");
ScreenWrapper weight_reset_9("Weight", KEY_NUM, "<-");
ScreenWrapper weight_init_10("Weight", KEY_NUM, "->");
ScreenWrapper empty = ScreenWrapper();

ScreenWrapper* getScreen(){
  switch(screen){
    // init screen
    case 7: return &name_init_7;
    case 8: return &age_init_8;
    case 10: return &weight_init_10;

    // main screen
    case 1: return &settings_list_1;
    case 2: return &alarm_list_2;
    case 3: return &audio_list_3;
    case 4: return &name_reset_4;
    case 5: return &age_reset_5;
    case 9: return &weight_reset_9;
    case 6: return &comms_list_6;
    default: return &empty;
  }
}

void showScreen(){
  delay(100);
  switch(screen){
    case 0:   duckDisplay_0();      break;
    case -1:  blankScreen();        break;
    default:  getScreen()->draw();  break;
  }
}

void screenSwitchHook(){
  switch(screen){
    case 4: case 7:  if(getScreen()->not_empty())    user_name = getScreen()->flush(); break;
    case 5: case 8:  if(getScreen()->not_empty())     user_age = getScreen()->flush().toInt(); break;
    case 9: case 10: if(getScreen()->not_empty())  user_weight = getScreen()->flush().toInt(); break;
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
  /* Actions that need to happen after a button click or game state transition*/ 
  if(fall_edge(3)){
    switch(screen){
      case 0:   growth=(growth+1)%3;                      break; // delete this later
      default:  getScreen()->moveBackward();              break;
    }showScreen();
  }else if(fall_edge(2)){
    switch(screen){
      default:  getScreen()->moveForward();               break;
    }showScreen();
  }else if(fall_edge(1)){
    switch(screen){
      case 0:   screenSwitchMap(0);                       break;
      case 1:   screenSwitchMap(getScreen()->getCurr());  break;
      default:  getScreen()->enter();                     break;
    }showScreen();
  }else if(fall_edge(0)){
    switch(screen){
      case 1:   screenSwitchMap(6);                       break;
      default:  screenSwitchMap(0);                       break;
    }showScreen();
  }else if(change_event()) showScreen();
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
