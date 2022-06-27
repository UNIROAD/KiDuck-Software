#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

bool prev2 = false, curr2 = false;
bool prev3 = false, curr3 = false;
bool prev4 = false, curr4 = false;
bool prev5 = false, curr5 = false;

bool fall_edge(int16_t pin_num, bool* prev, bool* curr){
  *prev = *curr;
  *curr = (digitalRead(pin_num))?true:false;
  return *prev && !*curr;
}

bool buttonPressDetect(){
  if(fall_edge(2, &prev2, &curr2)
      ||fall_edge(3, &prev3, &curr3)
      ||fall_edge(4, &prev4, &curr4)
      ||fall_edge(5, &prev5, &curr5)){
    return true;
  }
  return false;
}

void setButtonPinMode(){
  for(int i=2;i<6;i++) pinMode(i, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(BUTTON), cleartest, FALLING);
  //attachInterrupt(digitalPinToInterrupt(9), interruptButton2, FALLING);
}