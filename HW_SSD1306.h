#ifndef ARD
#include <SPI.h>
#include <Wire.h>
#define ARD
#endif


#ifndef HW_SSD
#define HW_SSD

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
    display.drawLine(div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADLESS),
                     div.position(-1, DIV_HEIGHT_DIRECTION, DIV_PADDED) + 1,
                     div.position(i%3+1, DIV_WIDTH_DIRECTION, DIV_PADLESS),
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


// function that displays List screen
void listDisplay(List* list, string title){
  Div div = Div((*list).getWidth(), (*list).getHeight(), 
                (*list).getXPos(), (*list).getYPos(), 
                1, (*list).getVisibleLen(), 2);
  string vis_text;

  display.clearDisplay();

  display.fillRect(div.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                  div.position((*list).getCursorPos(), DIV_HEIGHT_DIRECTION, DIV_PADDED),
                  div.getSectWidth(),
                  div.getSectHeight(),
                  SSD1306_WHITE);

  display.setTextSize(1); // Draw 1:1-scale text

  for(int i=0;i<(*list).getVisibleLen();i++){
    vis_text = (*list).getVisibleText(i);

    display.setTextColor((i==(*list).getCursorPos())?SSD1306_BLACK:SSD1306_WHITE);
    display.setCursor(div.textAllign(vis_text.length()*(TEXT_WIDTH+TEXT_PAD)-TEXT_PAD, 0, DIV_LEFT_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(TEXT_HEIGHT, i, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F(vis_text.c_str()));
    delay(1);
  }

  // Title
  display.drawLine(0, div.getSectHeight()+2, div.getSectWidth()+2, div.getSectHeight()+2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(F((" "+title).c_str()));

  navigationBarDisplay("^", "v", "o", "<-");

  display.display();
  delay(1);
}

void keyboardTextboxDisplay(Keyboard* keyboard, Textbox* textbox, string title){

  // keyboard display
  Div div_key = Div((*keyboard).getWidth(), (*keyboard).getHeight(), 
                (*keyboard).getXPos(), (*keyboard).getYPos(), 
                (*keyboard).getVisibleLen(), 1, 2);
  string vis_text;

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

  int len = (*keyboard).getVisibleLen();
  for(int i=0;i<len;i++){
    vis_text = (*keyboard).getVisibleText(i-len/2);

    display.setCursor(div_key.textAllign(TEXT_WIDTH, i, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div_key.textAllign(TEXT_HEIGHT-4, 3, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F(vis_text.c_str()));
    delay(1);
  }

  // textbox display
  Div div_text = Div((*textbox).getWidth(), (*textbox).getHeight(), 
                (*textbox).getXPos(), (*textbox).getYPos(), 1, 1, 2);
  
  display.drawLine(div_text.getXPos(), div_text.getYPos()+div_text.getHeight(),
                   div_text.getYPos()+div_text.getWidth(), div_text.getYPos()+div_text.getHeight(),
                   SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(div_text.getXPos(), div_text.getYPos()+3);
  display.print(F(((*textbox).getText()).c_str()));

  // Title
  Div div = Div(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 5, 2);
  display.drawLine(0, div.getSectHeight()+2, div.getSectWidth()+2, div.getSectHeight()+2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(F((" "+title).c_str()));


  navigationBarDisplay("<", ">", "o", "<-");

  display.display();
  delay(1);
}




/*



*/

#define DUCK_HEIGHT   0x32
#define DUCK_WIDTH    0x32
static const unsigned char PROGMEM duck_bmp[] =
{0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x07,0xe0,0x00,0x00,0x00
,0x00,0x00,0x38,0x1c,0x00,0x00,0x00
,0x00,0x00,0xc0,0x03,0x00,0x00,0x00
,0x00,0x01,0x00,0x00,0x80,0x00,0x00
,0x00,0x02,0x00,0x00,0x40,0x00,0x00
,0x18,0x04,0x00,0x00,0x20,0x00,0x00
,0x1f,0x88,0x00,0x00,0x20,0x00,0x00
,0x1c,0x78,0x00,0x00,0x10,0x00,0x00
,0x18,0x06,0x00,0x00,0x18,0x00,0x00
,0x1c,0x01,0x01,0xc0,0x18,0x00,0x00
,0x0e,0x01,0xc2,0xa0,0x18,0x00,0x00
,0x09,0x00,0x20,0x00,0x08,0x00,0x00
,0x04,0xc0,0x10,0x00,0x0c,0x00,0x00
,0x04,0x60,0x08,0x00,0x08,0x00,0x00
,0x02,0x18,0x0c,0x00,0x0c,0x00,0x00
,0x00,0x0e,0x06,0x00,0x0c,0x00,0x00
,0x00,0x83,0xc2,0x00,0x18,0x00,0x00
,0x00,0x80,0xc2,0x00,0x18,0x00,0x00
,0x01,0x00,0x74,0x00,0x18,0x00,0x00
,0x03,0x03,0xc4,0x00,0x10,0x00,0x00
,0x01,0xf8,0x08,0x00,0x20,0x00,0x00
,0x00,0x80,0x30,0x00,0x20,0x00,0x00
,0x00,0x60,0xc0,0x00,0x20,0x00,0x00
,0x00,0x1f,0x00,0x40,0x20,0x00,0x00
,0x00,0x04,0x00,0x40,0x20,0x00,0x00
,0x00,0x08,0x00,0x40,0x18,0x00,0x00
,0x00,0x08,0x00,0x40,0x18,0x70,0x00
,0x00,0x08,0x00,0x40,0x19,0x90,0x00
,0x00,0x10,0x00,0x20,0x0e,0x30,0x00
,0x00,0x10,0x00,0x20,0x0c,0x30,0x00
,0x00,0x10,0x00,0x10,0x0c,0x60,0x00
,0x00,0x10,0x00,0x08,0x06,0x60,0x00
,0x00,0x10,0x00,0x06,0x03,0xc0,0x00
,0x00,0x10,0x00,0x01,0xff,0x80,0x00
,0x00,0x08,0x00,0x00,0x03,0x00,0x00
,0x00,0x08,0x00,0x00,0x06,0x00,0x00
,0x00,0x04,0x00,0x00,0x1c,0x00,0x00
,0x00,0x02,0x00,0x00,0x70,0x00,0x00
,0x00,0x01,0x80,0x03,0x80,0x00,0x00
,0x00,0x00,0x78,0x1c,0x00,0x00,0x00
,0x00,0x00,0x0f,0xfc,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void duckDisplay_0(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 1, 1, 2);
  display.clearDisplay();
  display.drawBitmap(div.textAllign(DUCK_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                     div.textAllign(DUCK_WIDTH, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION), 
                     duck_bmp, DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);
  navigationBarDisplay(" ", " ", "menu", " ");
  display.display();
}


List listConstruct(string* temp, int size_list){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  string **temp_str = (string**)malloc(sizeof(string*)*size_list);

  for(int i=0;i<size_list;i++){
   temp_str[i] = &(temp[i]);
  }

  return List(SCREEN_WIDTH, div.multiSectSize(3, DIV_HEIGHT_DIRECTION, DIV_PADDED),
              0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), size_list, 3, temp_str);
}

Textbox textboxConstruct(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 7, 4, 2);
  return Textbox(div.multiSectSize(5, DIV_WIDTH_DIRECTION, DIV_PADLESS), div.getSectHeight(),
                 div.position(1, DIV_WIDTH_DIRECTION, DIV_PADLESS), div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), 10);
}

char keyEng[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
Keyboard engKeyboardConstruct(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  return Keyboard(SCREEN_WIDTH, div.getSectHeight(), 
                  0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), 27, 5, keyEng);
}

#endif
