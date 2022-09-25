#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

#ifndef GLST
#include <vector>

using namespace std;

// Game Datas
string user_name;
int user_age;

int character;
int growth;
int points;

bool previn[] = {false, false, false, false};
bool currin[] = {false, false, false, false};

void setButtonPinMode(){
  // pin 9, 8, 7, 6
  for(int i=6;i<10;i++) pinMode(i, INPUT_PULLUP);
}

bool fall_edge(int num){
  previn[num] = currin[num];
  currin[num] = (bool)digitalRead(num+6);
  return previn[num] && !currin[num];
}

bool t_flag=false;
unsigned long start_t, crit_t=100;

bool elapsed_time_check(){
    if(!t_flag){
        t_flag = true;
        start_t = millis();
    }else if(millis()-start_t>=crit_t){
        t_flag = false;
        return true;
    }
    return false;
}

#define GLST
#endif