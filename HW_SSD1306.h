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
void listDisplay(List* list){
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
  display.print(F(" Settings"));

  navigationBarDisplay("^", "v", "<-", "o");

  display.display();
  delay(1);
}

void keyboardDisplay(Keyboard* keyboard){
  Div div = Div((*keyboard).getWidth(), (*keyboard).getHeight(), 
                (*keyboard).getXPos(), (*keyboard).getYPos(), 
                (*keyboard).getVisibleLen(), 1, 2);
  string vis_text;

  display.clearDisplay();

  display.setTextSize(1); // Draw 1:1-scale text

  display.drawRoundRect(div.position(0, DIV_WIDTH_DIRECTION, DIV_PADDED),
                        div.position(3, DIV_HEIGHT_DIRECTION, DIV_PADDED),
                        div.getWidth(), div.getHeight(), 3, SSD1306_WHITE);
  
  int cursor_width = div.getSectWidth()*2/3;
  display.drawRoundRect(div.textAllign(cursor_width, 2, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                        div.position(3, DIV_HEIGHT_DIRECTION, DIV_PADDED)-2,
                        cursor_width, div.getHeight()+4, 3, SSD1306_WHITE);

  display.setTextColor(SSD1306_WHITE);

  int len = (*keyboard).getVisibleLen();
  for(int i=0;i<len;i++){
    vis_text = (*keyboard).getVisibleText(i-len/2);

    display.setCursor(div.textAllign(TEXT_WIDTH, i, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                      div.textAllign(TEXT_HEIGHT-4, 3, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION));
    display.print(F(vis_text.c_str()));
    delay(1);
  }

  navigationBarDisplay("<", ">", "<-", "o");

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

void duckDisplay(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT*4/5, 0, 0, 1, 1, 2);
  display.clearDisplay();
  display.drawBitmap(div.textAllign(DUCK_HEIGHT, 0, DIV_CENTER_ALLIGNMENT, DIV_WIDTH_DIRECTION),
                     div.textAllign(DUCK_WIDTH, 0, DIV_CENTER_ALLIGNMENT, DIV_HEIGHT_DIRECTION), 
                     duck_bmp, DUCK_WIDTH, DUCK_HEIGHT, SSD1306_WHITE);
  navigationBarDisplay("^", "v", "menu", "o");
  display.display();
}



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

char keytemp[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

Keyboard keyboardConstructTest(){
  Div div = Div(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 5, 2);
  // char **tempkey = (char**)malloc(sizeof(char*)*26);

  // for(int i=0;i<27;i++){
  //  tempkey[i] = &(keytemp[i]);
  // }
            
  Keyboard keyboard = Keyboard(SCREEN_WIDTH, div.getSectHeight(), 
                               0, div.position(1, DIV_HEIGHT_DIRECTION, DIV_PADLESS), 27, 5, keytemp);
  return keyboard;
}



bool buttonMapList(List* list){
  if(fall_edge(5, &prev5, &curr5)){
    (*list).moveBackward();
    return true;
  }
  else if(fall_edge(4, &prev4, &curr4)){
    (*list).moveForward();
    return true;
  }
  return false;
}

bool buttonMapKey(Keyboard* keyboard){
  if(fall_edge(5, &prev5, &curr5)){
    (*keyboard).moveBackward();
    return true;
  }
  else if(fall_edge(4, &prev4, &curr4)){
    (*keyboard).moveForward();
    return true;
  }
  return false;
}


#endif
