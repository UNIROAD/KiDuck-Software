#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

bool prev2 = false, curr2 = false;
bool prev3 = false, curr3 = false;
bool prev4 = false, curr4 = false;
bool prev5 = false, curr5 = false;

// int prevb = 0b0000, currb = 0b0000, pressb = 0;

bool fall_edge(int16_t pin_num, bool* prev, bool* curr){
  *prev = *curr;
  *curr = (digitalRead(pin_num))?true:false;
  return *prev && !*curr;
}

void setButtonPinMode(){
  for(int i=2;i<6;i++) pinMode(i, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(BUTTON), cleartest, FALLING);
  //attachInterrupt(digitalPinToInterrupt(9), interruptButton2, FALLING);
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

// int buttonPress(){
//   prevb = currb;
//   currb = (PORTD & 0b111100)>>2;
//   pressb = prevb & ~currb;
//   for(int i=0; i<4; i++){
//     if(1&(pressb>>i)) return i;
//   }
//   return 0;
// }
