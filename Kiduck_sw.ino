#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define BUTTON 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int16_t screen = 0;
bool prev2 = false;
bool curr2 = false;
bool prev3 = false;
bool curr3 = false;
bool prev4 = false;
bool curr4 = false;
bool prev5 = false;
bool curr5 = false;

void setup(){
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  for(int i=10;i<70;i++){
    display.drawPixel(10, i, SSD1306_WHITE);
  }

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  testrect();
  interruptButton();
  interruptButton2();
  testrect();
  //testdrawrect();
  //testfillrect();
  for(int i=2;i<6;i++) pinMode(i, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(BUTTON), cleartest, FALLING);
  //attachInterrupt(digitalPinToInterrupt(9), interruptButton2, FALLING);
}

void cleartest(){
  for(int i = 0; i < 2; i++){
    // LED 블링크 0.5초 켜고, 0.5초 끄고
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1);
    digitalWrite(LED_BUILTIN, LOW);
  }
  screen++;
  screen = screen%3;
  switch(screen){
    case 0:
      testrect();
      break;
    case 1:
      interruptButton();
      break;
    case 2:
      interruptButton2();
      break;
    default:
      testrect();
  }
  delay(100);
}
void interruptButton(){
  display.clearDisplay();
  int16_t x_num = 2;
  int16_t y_num = 3;
  int16_t pad = 3;
  int16_t h = (display.height()-pad)/y_num-pad;
  int16_t w = (display.width()-pad)/x_num-pad;

  display.fillRect(pad, pad+(y_num-1)*(h+pad), w, h,SSD1306_WHITE);
  display.drawRect(pad+(w+pad), pad+(y_num-1)*(h+pad), w, h,SSD1306_WHITE);
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("hi"));
  display.setCursor(20, 0);
  display.setTextColor(SSD1306_BLACK);
  display.print(F("\n\n\n\n\n\n   enter"));
  display.setTextColor(SSD1306_WHITE);
  display.println(F("     delete"));
  
  display.display();
  delay(1);
}
void interruptButton2(){
  display.clearDisplay();
  int16_t x_num = 2;
  int16_t y_num = 3;
  int16_t pad = 3;
  int16_t h = (display.height()-pad)/y_num-pad;
  int16_t w = (display.width()-pad)/x_num-pad;

  display.drawRect(pad, pad+(y_num-1)*(h+pad), w, h,SSD1306_WHITE);
  display.fillRect(pad+(w+pad), pad+(y_num-1)*(h+pad), w, h,SSD1306_WHITE);
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("hi"));
  display.setCursor(20, 0);
  display.print(F("\n\n\n\n\n\n   enter"));
  display.setTextColor(SSD1306_BLACK);
  display.println(F("     delete"));
  
  display.display();
  delay(1);
}

bool fall_edge(int16_t pin_num, bool* prev, bool* curr){
  *prev = *curr;
  *curr = (digitalRead(pin_num))?true:false;
  return *prev && !*curr;
}

void loop(){
  if(fall_edge(2, &prev2, &curr2)
      ||fall_edge(3, &prev3, &curr3)
      ||fall_edge(4, &prev4, &curr4)
      ||fall_edge(5, &prev5, &curr5)){
    cleartest();
  }
}

void testrect(void){
  display.clearDisplay();
  int16_t x_num = 2;
  int16_t y_num = 3;
  int16_t pad = 3;
  int16_t h = (display.height()-pad)/y_num-pad;
  int16_t w = (display.width()-pad)/x_num-pad;

  for(int16_t i=0;i<x_num;i++){
    display.drawRect(pad+i*(w+pad), pad+(y_num-1)*(h+pad), w, h,SSD1306_WHITE);
  }
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("hi"));
  display.setCursor(20, 0);
  display.println(F("\n\n\n\n\n\n   enter     delete"));
  
  display.display();
  delay(1);

}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/8; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}
