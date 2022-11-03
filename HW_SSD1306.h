#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Ingame_Mechanics.h"
#include "Global_State.h"
#include "GUI_elements.h"

#ifndef HW_SSD

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
}

void startupDisplay(){
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
  display.print(text);
  
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


// String length less than 2 is recommended
// draws on top of whatever is on the screen
void navigationBarDisplay(String str1, String str2, String str3, String str4){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  String temp_text[4] = {str1, str2, str3, str4};

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
    display.drawLine(div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADLESS),
                     div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED) + 1,
                     div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADLESS),
                     div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED) + div.getSectHeight() - 1,
                     SSD1306_BLACK);
    delay(1);
    
    display.setCursor(div.textAllign(temp_text[i].length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, i, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(TEXT_HEIGHT, -1, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(temp_text[i]);
    delay(1);
  }
  display.display();
  delay(1);
}




void blankScreen(){
  display.clearDisplay();
  display.display();
}


/*



*/

void duckDisplay(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 2, 1, 2);
  display.clearDisplay();
  // display.drawBitmap(div.textAllign(DUCK_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
  //                    div.textAllign(DUCK_WIDTH, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION), 
  //                    duck_bmp, DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);
  display.drawBitmap(div.textAllign(DUCK_WIDTH, 0, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                     div.textAllign(DUCK_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION), 
                     duck_img(), DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);
  navigationBarDisplay(" ", " ", "menu", " ");

  // Game State
  div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 2, 5, 2);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(div.textAllign(TEXT_WIDTH*10, 1, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                    div.textAllign(TEXT_HEIGHT, 1, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
  display.print("Steps: "+String(today_steps) + " ");

  // Name
  display.setCursor(div.textAllign(TEXT_WIDTH*(user_name.length()+2), 1, DIV_RIGHT_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                    div.textAllign(TEXT_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
  display.print(user_name+"  ");
  
  display.display();
}

class ListScreen: public Screen{
public:
  String title;
  List list;

  ListScreen(String title, String* stringlist, int size_list){
    this->title = title;
    this->list = listConstrict(stirnglist, size_list)
  }

  List listConstruct(String* temp, int size_list){
    Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
    String **temp_str = (String**)malloc(sizeof(String*)*size_list);

    for(int i=0;i<size_list;i++){
    temp_str[i] = &(temp[i]);
    }

    return List(SCREEN_WIDTH, div.multiSectSize(3, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), size_list, 3, temp_str);
  }

  void draw(){
    Div div = Div(this->list.getWidth(), this->list.getHeight(), 
                  this->list.getXPos(), this->list.getYPos(), 
                  1, this->list.getVisibleLen(), 2);
    String vis_text;

    display.clearDisplay();

    display.fillRect(div.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                    div.position(this->list.getCursorPos(), DIV_HEIGHT_DIRECTION, DIV_PADDED),
                    div.getSectWidth(),
                    div.getSectHeight(),
                    SSD1306_WHITE);

    display.setTextSize(1); // Draw 1:1-scale text

    for(int i=0;i<this->list.getVisibleLen();i++){
      vis_text = this->list.getVisibleText(i);

      display.setTextColor((i==this->list.getCursorPos())?SSD1306_BLACK:SSD1306_WHITE);
      display.setCursor(div.textAllign(vis_text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, 0, DIV_LEFT_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                        div.textAllign(TEXT_HEIGHT, i, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
      display.print(vis_text);
      delay(1);
    }

    // Title
    display.drawLine(0, div.getSectHeight()+2, div.getSectWidth()+2, div.getSectHeight()+2, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 2);
    display.print(" "+this->title);

    navigationBarDisplay("^", "v", "o", "<-");

    display.display();
    delay(1);
  }
};


#define KEY_ENG 27
#define KEY_NUM 11
char keyEng[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char keyNum[11] = "0123456789";

class KeyboardTextboxScreen: public Screen{
public:
  String title
  Textbox textbox;
  Keyboard keyboard;

  KeyboardTextboxScreen(String title, int keymode){
    this->title = title;
    this->textbox = this->textboxConstruct()
    this->keyboard = this->keyboardConstruct(keymode)
  }

  Textbox textboxConstruct(){
    Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 7, 4, 2);
    return Textbox(div.multiSectSize(5, DIV_WIDTH_DIRECTION, DIV_PADLESS), div.getSectHeight(),
                  div.position(1, DIV_WIDTH_DIRECTION, DIV_PADLESS), div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), 10);
  }

  Keyboard keyboardConstruct(int mode){
    Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
    return Keyboard(SCREEN_WIDTH, div.getSectHeight(), 
                    0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), mode, 5, 
                    (mode==KEY_ENG)?keyEng:keyNum);
  }

  void draw(){

    // keyboard display
    Div div_key = Div(this->keyboard.getWidth(), this->keyboard.getHeight(), 
                  this->keyboard.getXPos(), this->keyboard.getYPos(), 
                  this->keyboard.getVisibleLen(), 1, 2);
    String vis_text;

    display.clearDisplay();

    display.setTextSize(1); // Draw 1:1-scale text

    display.drawRoundRect(div_key.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                          div_key.position(3, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                          div_key.multiSectSize(5, DIV_WIDTH_DIRECTION, DIV_PADLESS), div_key.getHeight(), 3, SSD1306_WHITE);
    
    int cursor_width = div_key.getSectWidth()*2/3;
    display.drawRoundRect(div_key.textAllign(cursor_width, 2, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                          div_key.position(3, DIV_HEIGHT_DIRECTION, DIV_PADDED)-2,
                          cursor_width, div_key.getHeight()+4, 3, SSD1306_WHITE);

    display.setTextColor(SSD1306_WHITE);

    int len = this->keyboard.getVisibleLen();
    for(int i=0;i<len;i++){
      vis_text = this->keyboard.getVisibleText(i-len/2);

      display.setCursor(div_key.textAllign(TEXT_WIDTH, i, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                        div_key.textAllign(TEXT_HEIGHT-4, 3, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
      display.print(vis_text);
      delay(1);
    }

    // textbox display
    Div div_text = Div(this->textbox.getWidth(), this->textbox.getHeight(), 
                  this->textbox.getXPos(), this->textbox.getYPos(), 1, 1, 2);
    
    display.drawLine(div_text.getXPos(), div_text.getYPos()+div_text.getHeight(),
                    div_text.getYPos()+div_text.getWidth(), div_text.getYPos()+div_text.getHeight(),
                    SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(div_text.getXPos(), div_text.getYPos()+3);
    display.print(this->textbox.getText());

    // Title
    Div div = Div(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 5, 2);
    display.drawLine(0, div.getSectHeight()+2, div.getSectWidth()+2, div.getSectHeight()+2, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 2);
    display.print(" "+this->title);

    display.display();
    delay(1);
  }
};


#define HW_SSD
#endif
