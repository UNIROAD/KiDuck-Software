#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Ingame_Mechanics.h"
#include "Global_State.h"
#include "GUI_elements.h"
#include "Bluetooth.h"
#include "Friend_Meet.h"

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


class displaySequence{
public:
    Adafruit_SSD1306* disp;

    displaySequence(Adafruit_SSD1306* display)
    :disp(display){}

    Adafruit_SSD1306* dp(){return this->disp;}

    displaySequence display()                {(*dp()).display();           return *this;}
    displaySequence clearDisplay()           {(*dp()).clearDisplay();      return *this;}
    displaySequence delays(unsigned long ms) {delay(ms);                   return *this;}
    displaySequence setTextSize(int ts)      {(*dp()).setTextSize(ts);     return *this;}
    displaySequence setTextColor(int color)  {(*dp()).setTextColor(color); return *this;}
    displaySequence setCursor(int x, int y)  {(*dp()).setCursor(x, y);     return *this;}
    displaySequence print(String text)       {(*dp()).print(text);         return *this;}
    displaySequence print(char* text)        {(*dp()).print(text);         return *this;}
    displaySequence println(String text)     {(*dp()).println(text);       return *this;}
    displaySequence println(char* text)      {(*dp()).println(text);       return *this;}

    displaySequence drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)                    {(*dp()).drawLine(x1, y1, x2, y2, color);       return *this;}
    displaySequence drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rad, uint16_t color)     {(*dp()).drawRoundRect(x, y, w, h, rad, color); return *this;}
    displaySequence drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)                        {(*dp()).drawRect(x, y, w, h, color);           return *this;}
    displaySequence drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color)         {(*dp()).drawBitmap(x, y, bitmap, w, h, color); return *this;}
    displaySequence fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rad, uint16_t color)     {(*dp()).fillRoundRect(x, y, w, h, rad, color); return *this;}
    displaySequence fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)                        {(*dp()).fillRect(x, y, w, h, color);           return *this;}
    
    displaySequence drawText(Div div, String text, int textsize, int color, int posX, int posY, int allign){
        int w = TEXT_WIDTH;
        int h = TEXT_HEIGHT;
        int p = TEXT_PAD;
        int l = text.length();
        int ts = textsize;

        return (*this).setTextSize(ts)
                      .setTextColor(color)
                      .setCursor(div.allign(l*ts*(w+p)-p, posX,     allign, DIV_DIR_W),
                                 div.allign(        ts*h, posY, DIV_ALGN_C, DIV_DIR_H))
                      .print(text);
    }

    displaySequence selectRect(bool sel, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
      if(sel) (*dp()).fillRect(x, y, w, h, color);
      else    (*dp()).drawRect(x, y, w, h, color);
      return *this;
    }

    //############ UI Components ############//
    /*String length less than 2 is recommended
    draws on top of whatever is on the screen*/
    displaySequence navigationBarDisplay(String str1, String str2, String str3, String str4){
      String temp_text[4] = {str1, str2, str3, str4};

      (*this).fillRoundRect(globalDiv.pos( 0, DIV_DIR_W, DIV_PAD_O),
                            globalDiv.pos(-1, DIV_DIR_H, DIV_PAD_O),
                            globalDiv.getSectWidth(),
                            globalDiv.getSectHeight(),
                            globalDiv.getSectHeight()/2,  // radius of round edge
                            SSD1306_WHITE);
      
      Div div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 4, 5, 2);

      for(int i=0;i<4;i++){
        (*this).drawLine(div.pos(i%3+1, DIV_DIR_W, DIV_PAD_X),
                         div.pos(   -1, DIV_DIR_H, DIV_PAD_O) + 1,
                         div.pos(i%3+1, DIV_DIR_W, DIV_PAD_X),
                         div.pos(   -1, DIV_DIR_H, DIV_PAD_O) + div.getSectHeight() - 1,
                         SSD1306_BLACK)
               .delays(1)
               .drawText(div, temp_text[i], 1, SSD1306_BLACK, i, -1, DIV_ALGN_C)
               .delays(1);
      }
      (*this).display()
             .delays(1);
      return *this;
    }

    displaySequence titleDisplay(String title){
        return (*this).drawLine(0, 
                                globalDiv.getSectHeight()+2, 
                                globalDiv.getSectWidth()+2, 
                                globalDiv.getSectHeight()+2, 
                                SSD1306_WHITE)
                      .drawText(globalDiv, " "+title, 1, SSD1306_WHITE, 0, 0, DIV_ALGN_L);
    }
};

displaySequence disp(&display);

//########################## UI Component Functions ##########################//
// function that displays set of rectangle Button
void _drawButton(bool selected, Div div, int x_num, int y_num, String text){
  disp.selectRect(selected,
                  div.pos(x_num, DIV_DIR_W, DIV_PAD_O),
                  div.pos(y_num, DIV_DIR_H, DIV_PAD_O),
                  div.getSectWidth(),
                  div.getSectHeight(),
                  SSD1306_WHITE)
      .drawText(div, text, 1, (selected)?SSD1306_BLACK:SSD1306_WHITE, x_num, y_num, DIV_ALGN_C);
}


void rectButtonSetDisplay(int num){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 2, 4, 3);
  display.clearDisplay();
  
  _drawButton((bool)(num%2), div, 0, -2, "enter");
  _drawButton((bool)(num/2), div, 1, -2, "delete");
  
  disp.setTextSize(1)
      .setTextColor(SSD1306_WHITE)
      .setCursor(10, 0)
      .println("hi")
      .display()
      .delays(1);
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
    
    disp.clearDisplay()
        .drawText(div, "UNIROAD", 3, SSD1306_WHITE, 0, 0, DIV_ALGN_C)
        .delays(1)
        .display()
        .delays(2000)
        .clearDisplay();
}


void blankScreen(){
  disp.clearDisplay()
      .display();
}

void textScreen(String text, String str1=" ", String str2=" ", String str3=" ", String str4=" "){
  Div div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 1, 1, 2);
  disp.clearDisplay()
      .drawText(div, text, 1, SSD1306_WHITE, 0, 0, DIV_ALGN_C)
      .navigationBarDisplay(str1, str2, str3, str4)
      .display();
}

void duckDisplay_0(){
  Div div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 2, 1, 2);
  Div div2(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 2, 5, 2);

  disp.clearDisplay();
  display.drawBitmap(div.allign( DUCK_WIDTH, 0, DIV_ALGN_C, DIV_DIR_W), 
                     div.allign(DUCK_HEIGHT, 0, DIV_ALGN_C, DIV_DIR_H), 
                     duck_img(), DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);

  disp.drawText(div2, "  "+user_name, 1, SSD1306_WHITE, 1, 0, DIV_ALGN_R)                           // Name
      .drawText(div2, "point: "+String(points) + " ", 1, SSD1306_WHITE, 1, 1, DIV_ALGN_L)           // Point
      .drawText(div2, "Steps: "+String(today_steps) + " ", 1, SSD1306_WHITE, 1, 2, DIV_ALGN_L)      // Steps
      .drawText(div2, "Water: "+String((int)today_water) + " ", 1, SSD1306_WHITE, 1, 3, DIV_ALGN_L)      // Water
      .drawText(div2, "Meet : "+String(today_met_count) + " ", 1, SSD1306_WHITE, 1, 4, DIV_ALGN_L) // Meet
      .navigationBarDisplay(" ", "meet", "menu", " ")
      .display();
}


void syncApp_9_10(){
  textScreen("Synchronizing...");

  bleSetup();
  while(syncApp());
  
  textScreen("Complete", " ", " ", " ", "<-");
}

void syncBottle_11(){textScreen("Sync with Bottle", " ", " ", " ", "<-");}

void friendMeet_12(){textScreen("Friend Meet", "^", "v", " ", "<-");}

void friendMeet_13_14(int mode){
  String str = "Friend Meet " + String(mode?"Recv":"Send");
  textScreen(str+" Mode");
  
  switch(meet(mode)){
    case 0:  textScreen(str+" Complete"); break;
    case -1: textScreen("You already met this friend today"); break;
    case -2: textScreen("Force Exit"); break;
    default: textScreen("Friend Meet"); break;
  }
}


//########################## Screen Classes ##########################//
class ListScreen{
public:
  String title;
  List list;

  ListScreen(String title, String* stringlist, int size_list)
  : title(title), 
    list(listConstruct(stringlist, size_list)){}

  List listConstruct(String* temp, int size_list){
    String **temp_str = (String**)malloc(sizeof(String*)*size_list);

    for(int i=0;i<size_list;i++) 
      temp_str[i] = &(temp[i]);

    return List(SCREEN_WIDTH, 
                globalDiv.multiSectSize(3, DIV_DIR_H, DIV_PAD_O), 0, 
                globalDiv.pos(1, DIV_DIR_H, DIV_PAD_X), 
                size_list, 3, temp_str);
  }

  displaySequence listDraw(){
    List ls = this->list;
    Div div = ls.getDiv(1, ls.getVisibleLen(), 2);

    disp.fillRect(div.pos(0, DIV_DIR_W, DIV_PAD_O),
                  div.pos(ls.getCursorPos(), DIV_DIR_H, DIV_PAD_O), 
                  div.getSectWidth(), 
                  div.getSectHeight(), 
                  SSD1306_WHITE);

    // each entry
    for(int i=0;i<ls.getVisibleLen();i++)
      disp.drawText(div, ls.getVisibleText(i), 1, 
                   (i==ls.getCursorPos())?SSD1306_BLACK:SSD1306_WHITE,  
                   0, i, DIV_ALGN_L)
          .delays(1);

    return disp;
  }

  void draw(){
    disp.clearDisplay();
         listDraw()
        .titleDisplay(this->title)
        .navigationBarDisplay("^", "v", "o", "<-")
        .display()
        .delays(1);
  }
};


#define KEY_ENG 27
#define KEY_NUM 11
char keyEng[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char keyNum[11] = "0123456789";

class KeyboardTextboxScreen{
public:
  String title;
  Textbox textbox;
  Keyboard keyboard;
  String navKey4;
  int keymode;

  KeyboardTextboxScreen(String title, int keymode, int length, String navKey4)
  : title(title), 
    textbox(textboxConstruct(length)), 
    keyboard(keyboardConstruct(keymode)), 
    navKey4(navKey4),
    keymode(keymode){}

  Textbox textboxConstruct(int length){
    Div div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 7, 4, 2);
    return Textbox(div.multiSectSize(5, DIV_DIR_W, DIV_PAD_X), 
                   div.getSectHeight(), 
                   div.pos(1, DIV_DIR_W, DIV_PAD_X), 
                   div.pos(1, DIV_DIR_H, DIV_PAD_X), length);
  }

  Keyboard keyboardConstruct(int mode){
    return Keyboard(SCREEN_WIDTH, 
                    globalDiv.getSectHeight(), 0, 
                    globalDiv.pos(1, DIV_DIR_H, DIV_PAD_X), 
                    mode, 5, (mode==KEY_ENG)?keyEng:keyNum);
  }

  displaySequence keyboardDraw(){
    Keyboard kb = this->keyboard;
    int len = kb.getVisibleLen();
    Div kb_div = kb.getDiv(len, 1, 2);
    int cursor_width = kb_div.getSectWidth()*2/3;
    disp.drawRoundRect(kb_div.pos(0, DIV_DIR_W, DIV_PAD_O), 
                      kb_div.pos(3, DIV_DIR_H, DIV_PAD_O)-1, 
                      kb_div.multiSectSize(5, DIV_DIR_W, DIV_PAD_X), 
                      kb_div.getHeight(), 3, SSD1306_WHITE)
        .drawRoundRect(kb_div.allign(cursor_width, 2, DIV_ALGN_C, DIV_DIR_W), 
                      kb_div.pos(3, DIV_DIR_H, DIV_PAD_O)-3, 
                      cursor_width, 
                      kb_div.getHeight()+4, 3, SSD1306_WHITE);
    for(int i=0;i<len;i++){
      disp.drawText(kb_div, String(kb.getVisibleText(i-len/2)), 1 , SSD1306_WHITE, i, 3, DIV_ALGN_C)
          .delays(1);
    }
    return disp;
  }

  displaySequence textboxDraw(){
    Textbox  tb = this->textbox;
    Div tb_div = tb.getDiv(1, 1, 2);
    disp.drawText(tb_div, tb.getText(), 1, SSD1306_WHITE, 0, 0, DIV_ALGN_C)
        .drawLine(tb_div.getXPos(), 
                  tb_div.getYPos()+tb_div.getHeight(), 
                  tb_div.getYPos()+tb_div.getWidth(), 
                  tb_div.getYPos()+tb_div.getHeight(), 
                  SSD1306_WHITE);
    return disp;
  }

  void draw(){
    disp.clearDisplay();
        keyboardDraw();
        textboxDraw()
        .titleDisplay(this->title)
        .navigationBarDisplay("<", ">", "o", this->navKey4)
        .display()
        .delays(1);
  }
};



String empty_string_list[1] = {""};
ListScreen empty_listscreen("", empty_string_list, 1);
KeyboardTextboxScreen empty_ktscreen("", 0, 0, "");

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

  ScreenWrapper(String title, int keymode, int length, String navKey4)
  : type(KTS), 
    ls(empty_listscreen),
    kts(KeyboardTextboxScreen(title, keymode, length, navKey4)){}

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
      case KTS: return kts.textbox.not_empty();
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

