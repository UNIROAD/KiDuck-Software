#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

bool previn[] = {false, false, false, false};
bool currin[] = {false, false, false, false};

void setButtonPinMode(){
  for(int i=2;i<6;i++) pinMode(i, INPUT_PULLUP);
}

// pin 5, 4, 3, 2
bool fall_edge(int num){
  previn[num] = currin[num];
  currin[num] = (digitalRead(num+2))?true:false;
  return previn[num] && !currin[num];
}