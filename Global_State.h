#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

bool previn[] = {false, false, false, false};
bool currin[] = {false, false, false, false};

string user_name;
int user_age;

void setButtonPinMode(){
  for(int i=6;i<10;i++) pinMode(i, INPUT_PULLUP);
}

// pin 5, 4, 3, 2
bool fall_edge(int num){
  previn[num] = currin[num];
  currin[num] = (bool)digitalRead(num+6);
  return previn[num] && !currin[num];
}
