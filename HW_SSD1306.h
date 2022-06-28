#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// 20 * 4 texts
#define TEXT_WIDTH 5 // Text width, in pixels
#define TEXT_HEIGHT 7 // Text height, in pixels
#define TEXT_PAD 1

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define BUTTON 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void SSD1306_Setup(){
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!  
    //display.display();
    //delay(2000); // Pause for 2 seconds

    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...
    // Clear the buffer
    display.clearDisplay();
}

void sbutton(bool selected, Div div, int x_num, int y_num, String text){
  if(selected){
   display.fillRect(div.position(x_num, DIV_WIDTH_DIRECTION),
                   div.position(y_num, DIV_HEIGHT_DIRECTION),
                   div.sect_width,
                   div.sect_height,
                   SSD1306_WHITE);
  }
  else{
   display.drawRect(div.position(x_num, DIV_WIDTH_DIRECTION),
                 div.position(y_num, DIV_HEIGHT_DIRECTION),
                 div.sect_width,
                 div.sect_height,
                 SSD1306_WHITE);
  }
  display.setTextSize(1); // Draw 1:1-scale text
  display.setTextColor((selected)?SSD1306_BLACK:SSD1306_WHITE);
  display.setCursor(div.text_center_pos(text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, x_num, DIV_WIDTH_DIRECTION),
                    div.text_center_pos(TEXT_HEIGHT, y_num, DIV_HEIGHT_DIRECTION));
  display.print(F(text.c_str()));
  
}


/*
    Draw a single pixel in white
    
    for(int i=10;i<70;i++){
        display.drawPixel(10, i, SSD1306_WHITE);
    }


*/

// class ButtonDisplay{
// protected:
// };


void brect(int num){
  display.clearDisplay();
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 3, 3);
  
  sbutton((bool)(num%2), div, 0, -1, "enter");
  sbutton((bool)(num/2), div, 1, -1, "delete");
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("hi"));
  
  display.display();
  delay(1);
}
