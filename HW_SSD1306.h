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

// void SSD1306_Setup(){
//     // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//     if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
//         Serial.println(F("SSD1306 allocation failed"));
//         for(;;); // Don't proceed, loop forever
//     }

//     // Show initial display buffer contents on the screen --
//     // the library initializes this with an Adafruit splash screen.
//     // Show the display buffer on the screen. You MUST call display() after
//     // drawing commands to make them visible on screen!  
//     display.display();
//     delay(2000); // Pause for 2 seconds

//     // display.display() is NOT necessary after every single drawing command,
//     // unless that's what you want...rather, you can batch up a bunch of
//     // drawing operations and then update the screen all at once by calling
//     // display.display(). These examples demonstrate both approaches...
//     // Clear the buffer
//     display.clearDisplay();
// }

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
    display.clearDisplay();

    Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1, 0);
    int textsize = 3;
    display.setTextSize(textsize);

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(div.textAllign(7*textsize*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, 0, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(textsize*TEXT_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F("UNIROAD"));
    delay(1);

    display.display();
    delay(2000); // Pause for 2 seconds

    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...
    // Clear the buffer
    display.clearDisplay();
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


// function that displays set of rectangle Button
void _sbutton(bool selected, Div div, int x_num, int y_num, String text){
  if(selected){
   display.fillRect(div.position(x_num, DIV_WIDTH_DIRECTION, DIV_PADDED),
                   div.position(y_num, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                   div.getSectWidth(),
                   div.getSectHeight(),
                   SSD1306_WHITE);
  }
  else{
   display.drawRect(div.position(x_num, DIV_WIDTH_DIRECTION, DIV_PADDED),
                 div.position(y_num, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                 div.getSectWidth(),
                 div.getSectHeight(),
                 SSD1306_WHITE);
  }
  display.setTextSize(1); // Draw 1:1-scale text
  display.setTextColor((selected)?SSD1306_BLACK:SSD1306_WHITE);
  display.setCursor(div.textAllign(text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, x_num, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                    div.textAllign(TEXT_HEIGHT, y_num, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
  display.print(F(text.c_str()));
  
}


void rectButtonSetDisplay(int num){
  display.clearDisplay();
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 2, 4, 3);
  
  _sbutton((bool)(num%2), div, 0, -2, "enter");
  _sbutton((bool)(num/2), div, 1, -2, "delete");
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("hi"));
  
  display.display();
  delay(1);
}
/*
int alpha = 0;

// function that displays List screen
void listDisplay(List list){
  Div div = Div(list.getWidth(), list.getHeight(), 1, list.getVisibleLen(), 3);
  string vis_text;
  
  display.clearDisplay();

  alpha = (alpha+1)%3;
  
  display.fillRect(div.position(0, DIV_WIDTH_DIRECTION),
                  div.position(alpha, DIV_HEIGHT_DIRECTION),//list.getCursorPos(), DIV_HEIGHT_DIRECTION),
                  div.getSectWidth(),
                  div.getSectHeight(),
                  SSD1306_WHITE);

  display.setTextSize(1); // Draw 1:1-scale text

  for(int i=0;i<list.getVisibleLen();i++){
    vis_text = list.getVisibleText(i);

    display.setTextColor((i==list.getCursorPos())?SSD1306_BLACK:SSD1306_WHITE);
    display.setCursor(div.text_center_pos(vis_text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, 0, DIV_WIDTH_DIRECTION),
                      div.text_center_pos(TEXT_HEIGHT, i, DIV_HEIGHT_DIRECTION));
    //display.print(F(vis_text.c_str()));
    delay(1);
  }
  display.display();
  delay(1);
}
*/

// string length less than 2 is recommended
// draws on top of whatever is on the screen
void navigationBarDisplay(string str1, string str2, string str3, string str4){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  string temp_text[4] = {str1, str2, str3, str4};

  display.fillRoundRect(div.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                        div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                        div.getSectWidth(),
                        div.getSectHeight(),
                        div.getSectHeight()/2,  // radius of round edge
                        SSD1306_WHITE);
  
  div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 4, 5, 2);

  display.setTextSize(1); // Draw 1:1-scale text
  display.setTextColor(SSD1306_BLACK);

  for(int i=0;i<4;i++){
    display.drawLine(div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADDED),
                     div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED) + 1,
                     div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADDED),
                     div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED) + div.getSectHeight() - 1,
                     SSD1306_BLACK);
    delay(1);
    
    display.setCursor(div.textAllign(temp_text[i].length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, i, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(TEXT_HEIGHT, -1, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F(temp_text[i].c_str()));
    delay(1);
  }
  display.display();
  delay(1);
}



/*



*/

string temptemp[5] = {" 1. Alarm",
                     " 2. Audio",
                     " 3. Name reset",
                     " 4. Age reset",
                     " 5. Comms"};

List listConstructTest(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  string **temp_str = (string**)malloc(sizeof(string*)*5);

  for(int i=0;i<5;i++){
   temp_str[i] = &(temptemp[i]);
  }
            
  List list = List(SCREEN_WIDTH, div.multiSectSize(3, DIV_HEIGHT_DIRECTION),
                   0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), 5, 3, temp_str);
  return list;
}


/*
List buttonMapList(List list){
  if(fall_edge(2, &prev2, &curr2)){
    list.moveBackward();
  }
  else if(fall_edge(3, &prev3, &curr3)){
    list.moveForward();
  }
  return list;
}
*/
