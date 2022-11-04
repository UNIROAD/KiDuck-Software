#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Ingame_Mechanics.h"
#include "Global_State.h"
#include "GUI_elements.h"

#ifndef HW_SSD

using namespace std;

//########################## SSD1306 HW Info ##########################//
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
Div globalDiv(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);

//########################## UI Component Functions ##########################//
void drawText(Div div, String text, int textsize, int color, int posX, int posY, int allign){
    int w = TEXT_WIDTH;
    int h = TEXT_HEIGHT;
    int p = TEXT_PAD;
    int l = text.length();
    int ts = textsize;

    display.setTextSize(ts);
    display.setTextColor(color);
    display.setCursor(div.allign(l*ts*(w+p)-p, posX,     allign, DIV_DIR_W),
                      div.allign(        ts*h, posY, DIV_ALGN_C, DIV_DIR_H));
    display.print(text);
}

// String length less than 2 is recommended
// draws on top of whatever is on the screen
void navigationBarDisplay(String str1, String str2, String str3, String str4){
  String temp_text[4] = {str1, str2, str3, str4};

  display.fillRoundRect(globalDiv.pos( 0, DIV_DIR_W, DIV_PAD_O),
                        globalDiv.pos(-1, DIV_DIR_H, DIV_PAD_O),
                        globalDiv.getSectWidth(),
                        globalDiv.getSectHeight(),
                        globalDiv.getSectHeight()/2,  // radius of round edge
                        SSD1306_WHITE);
  
  Div div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 4, 5, 2);


  for(int i=0;i<4;i++){
    display.drawLine(div.pos(i%3+1, DIV_DIR_W, DIV_PAD_X),
                     div.pos(   -1, DIV_DIR_H, DIV_PAD_O) + 1,
                     div.pos(i%3+1, DIV_DIR_W, DIV_PAD_X),
                     div.pos(   -1, DIV_DIR_H, DIV_PAD_O) + div.getSectHeight() - 1,
                     SSD1306_BLACK);
    delay(1);
    drawText(div, temp_text[i], 1, SSD1306_BLACK, i, -1, DIV_ALGN_C);
    delay(1);
  }
  display.display();
  delay(1);
}

void titleDisplay(String title){
    display.drawLine(0, 
                    globalDiv.getSectHeight()+2, 
                    globalDiv.getSectWidth()+2, 
                    globalDiv.getSectHeight()+2, 
                    SSD1306_WHITE);
    drawText(globalDiv, " "+title, 1, SSD1306_WHITE, 0, 0, DIV_ALGN_L);
}

// function that displays set of rectangle Button
void _sbutton(bool selected, Div div, int x_num, int y_num, String text){
  if(selected){
   display.fillRect(div.pos(x_num, DIV_DIR_W, DIV_PAD_O),
                   div.pos(y_num, DIV_DIR_H, DIV_PAD_O),
                   div.getSectWidth(),
                   div.getSectHeight(),
                   SSD1306_WHITE);
  }
  else{
   display.drawRect(div.pos(x_num, DIV_DIR_W, DIV_PAD_O),
                    div.pos(y_num, DIV_DIR_H, DIV_PAD_O),
                    div.getSectWidth(),
                    div.getSectHeight(),
                    SSD1306_WHITE);
  }
  drawText(div, text, 1, (selected)?SSD1306_BLACK:SSD1306_WHITE, x_num, y_num, DIV_ALGN_C);
  
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



//########################## Screen Functions ##########################//


void SSD1306_Setup(){
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
}

void startupDisplay(){
    Div div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1, 0);

    display.clearDisplay();
    drawText(div, "UNIROAD", 3, SSD1306_WHITE, 0, 0, DIV_ALGN_C); delay(1);
    display.display(); delay(2000);
    display.clearDisplay();
}


void blankScreen(){
  display.clearDisplay();
  display.display();
}

void duckDisplay_0(){
  Div div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 2, 1, 2);
  display.clearDisplay();
  display.drawBitmap(div.allign( DUCK_WIDTH, 0, DIV_ALGN_C, DIV_DIR_W),
                     div.allign(DUCK_HEIGHT, 0, DIV_ALGN_C, DIV_DIR_H), 
                     duck_img(), DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);

  div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 2, 5, 2);
  drawText(div, "Steps: "+String(today_steps) + " ", 1, SSD1306_WHITE, 1, 1, DIV_ALGN_C);  // Game State
  drawText(div, user_name+"  ", 1, SSD1306_WHITE, 1, 0, DIV_ALGN_R);                       // Name
  
  navigationBarDisplay(" ", " ", "menu", " ");
  display.display();
}


//########################## Screen classes ##########################//
class ListScreen: public Screen{
public:
  List list;

  ListScreen(String title, String* stringlist, int size_list)
  : Screen(title), 
    list(listConstruct(stringlist, size_list)){}

  List listConstruct(String* temp, int size_list){
    String **temp_str = (String**)malloc(sizeof(String*)*size_list);

    for(int i=0;i<size_list;i++)
      temp_str[i] = &(temp[i]);

    return List(SCREEN_WIDTH, globalDiv.multiSectSize(3, DIV_DIR_H, DIV_PAD_O),
                0, globalDiv.pos(1, DIV_DIR_H, DIV_PAD_X), size_list, 3, temp_str);
  }

  void draw(){
    List ls = this->list;
    Div div = ls.getDiv(1, ls.getVisibleLen(), 2);

    display.clearDisplay();
    display.fillRect(div.pos(                0, DIV_DIR_W, DIV_PAD_O),
                     div.pos(ls.getCursorPos(), DIV_DIR_H, DIV_PAD_O),
                     div.getSectWidth(), div.getSectHeight(), SSD1306_WHITE);

    // each entry
    for(int i=0;i<ls.getVisibleLen();i++){
      drawText(div, ls.getVisibleText(i), 1, (i==ls.getCursorPos())?SSD1306_BLACK:SSD1306_WHITE,  0, i, DIV_ALGN_L); delay(1);
    }

    titleDisplay(this->title);
    navigationBarDisplay("^", "v", "o", "<-");

    display.display(); delay(1);
  }
};


#define KEY_ENG 27
#define KEY_NUM 11
char keyEng[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char keyNum[11] = "0123456789";

class KeyboardTextboxScreen: public Screen{
public:
  Textbox textbox;
  Keyboard keyboard;
  String navKey4;
  int keymode;

  KeyboardTextboxScreen(String title, int keymode, String navKey4)
  : Screen(title), 
    textbox(this->textboxConstruct()), 
    keyboard(this->keyboardConstruct(keymode)), 
    navKey4(navKey4), 
    keymode(keymode){}

  Textbox textboxConstruct(){
    Div div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 7, 4, 2);
    return Textbox(div.multiSectSize(5, DIV_DIR_W, DIV_PAD_X), div.getSectHeight(),
                   div.pos(1, DIV_DIR_W, DIV_PAD_X), div.pos(1, DIV_DIR_H, DIV_PAD_X), 10);
  }

  Keyboard keyboardConstruct(int mode){
    return Keyboard(SCREEN_WIDTH, globalDiv.getSectHeight(), 
                    0, globalDiv.pos(1, DIV_DIR_H, DIV_PAD_X), mode, 5, 
                    (mode==KEY_ENG)?keyEng:keyNum);
  }

  void draw(){
    Keyboard kb = this->keyboard;
    Textbox  tb = this->textbox;
    int len = kb.getVisibleLen();
    Div kb_div = kb.getDiv(len, 1, 2);
    int cursor_width = kb_div.getSectWidth()*2/3;
    Div tb_div = tb.getDiv(1, 1, 2);

    // keyboard display
    display.clearDisplay();
    display.drawRoundRect(kb_div.pos(0, DIV_DIR_W, DIV_PAD_O),
                          kb_div.pos(3, DIV_DIR_H, DIV_PAD_O)-1,
                          kb_div.multiSectSize(5, DIV_DIR_W, DIV_PAD_X), kb_div.getHeight(), 3, SSD1306_WHITE);
    
    display.drawRoundRect(kb_div.allign(cursor_width, 2, DIV_ALGN_C, DIV_DIR_W),
                          kb_div.pos(3, DIV_DIR_H, DIV_PAD_O)-3,
                          cursor_width, kb_div.getHeight()+4, 3, SSD1306_WHITE);

    for(int i=0;i<len;i++){
      drawText(kb_div, String(kb.getVisibleText(i-len/2)), 1 , SSD1306_WHITE, i, 3, DIV_ALGN_C); delay(1);
    }

    // textbox display
    display.drawLine(tb_div.getXPos(), tb_div.getYPos()+tb_div.getHeight(),
                    tb_div.getYPos()+tb_div.getWidth(), tb_div.getYPos()+tb_div.getHeight(),
                    SSD1306_WHITE);
    drawText(tb_div, tb.getText(), 1, SSD1306_WHITE, 0, 0, DIV_ALGN_L);

    titleDisplay(this->title);
    navigationBarDisplay("<", ">", "o", this->navKey4);

    display.display();
    delay(1);
  }
};


String empty_string_list[1] = {""};
ListScreen empty_listscreen("", empty_string_list, 1);
KeyboardTextboxScreen empty_ktscreen("", 0, "");

#define EMPTY 0
#define LS 1
#define KTS 2
class ScreenWrapper{
public:
  int type;
  ListScreen ls;
  KeyboardTextboxScreen kts;
  
  //################ Constructors ################//
  ScreenWrapper(String title, String* stringlist, int size_list)
  : type(LS), 
    ls(ListScreen(title, stringlist, size_list)), 
    kts(empty_ktscreen){}

  ScreenWrapper(String title, int keymode, String navKey4)
  : type(KTS), 
    ls(empty_listscreen),
    kts(KeyboardTextboxScreen(title, keymode, navKey4)){}

  ScreenWrapper()
  : type(EMPTY), 
    ls(empty_listscreen), 
    kts(empty_ktscreen){}

  //################ Method Chooser ################//
  void draw(){
    switch(this->type){
      case LS: ls.draw(); break;
      case KTS: kts.draw(); break;
      default: break;
    }    
  }
  int getCurr(){
    switch(this->type){
      case LS: return ls.list.getCurr();
      default: return 0;
    }
  }
  String flush(){
    switch(this->type){
      case KTS: return kts.textbox.flush();
      default: return "";
    }
  }
  bool not_empty(){
    switch(this->type){
      case KTS: kts.textbox.not_empty(); break;
      default: return false;
    }
  }
  void enter(){
    switch(this->type){
      case KTS: kts.keyboard.enter(&kts.textbox); break;
      default: break;
    }
  }
  void moveBackward(){
    switch(this->type){
      case LS: ls.list.moveBackward(); break;
      case KTS: kts.keyboard.moveBackward(); break;
      default: break;
    }
  }
  void moveForward(){
    switch(this->type){
      case LS: ls.list.moveForward(); break;
      case KTS: kts.keyboard.moveForward(); break;
      default: break;
    }
  }
};

#define HW_SSD
#endif
