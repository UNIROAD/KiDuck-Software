#include <SPI.h>
#include <Wire.h>
#include <ArduinoSTL.h>

#ifndef GLST
using namespace std;

//############# Game Datas #############//
String user_name;
int user_age;
int user_weight;

int character;
int growth = 0;

//############# Pin Input Related #############//
bool previn[] = {false, false, false, false};
bool currin[] = {false, false, false, false};

void setButtonPinMode(){
  // pin 9, 8, 7, 6
  for(int i=6;i<10;i++) pinMode(i, INPUT_PULLUP);
}

bool fall_edge(int num){
  previn[num] = currin[num];
  currin[num] = (bool)digitalRead(num+6);
  return previn[num] && !currin[num];
}


//############# Clock #############//
class elapsed_time{
protected:
  bool t_flag;
  unsigned long start_t, crit_t;
public:
  elapsed_time(int crit_t){
    this->t_flag=false;
    this->crit_t = crit_t;
  }

  bool elapsed_time_check(){
    if(!this->t_flag){
        this->t_flag = true;
        this->start_t = millis();
    }else if(millis()-this->start_t>=this->crit_t){
        this->t_flag = false;
        return true;
    }
    return false;
  }
};



//############# Duck Image #############//
#define DUCK_HEIGHT   0x32
#define DUCK_WIDTH    0x32
static const unsigned char PROGMEM duck_0_bmp[]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x7f,0xfc,0x00,0x00,0x00
,0x00,0x00,0x7f,0xfc,0x00,0x00,0x00
,0x00,0x07,0xff,0xff,0x80,0x00,0x00
,0x00,0x07,0xff,0xff,0x80,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x3f,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x1f,0xff,0xff,0xff,0xff,0xe0,0x00
,0x03,0xff,0xff,0xff,0xff,0xe0,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x01,0xff,0xff,0xff,0xff,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x3f,0xff,0xff,0xf8,0x00,0x00
,0x00,0x07,0xff,0xff,0x80,0x00,0x00
,0x00,0x07,0xff,0xff,0x80,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM duck_1_bmp[] =
{0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x1f,0xfc,0x00,0x00,0x00
,0x00,0x00,0x1f,0xfc,0x00,0x00,0x00
,0x00,0x01,0xe0,0x07,0x80,0x00,0x00
,0x00,0x03,0x00,0x00,0x60,0x00,0x00
,0x00,0x06,0x00,0x00,0x60,0x00,0x00
,0x00,0x0c,0x00,0x00,0x18,0x00,0x00
,0x00,0x0c,0x00,0x00,0x18,0x00,0x00
,0x00,0x0c,0x00,0x00,0x18,0x00,0x00
,0x00,0x30,0x00,0x00,0x06,0x00,0x00
,0x00,0x30,0x18,0x0c,0x06,0x00,0x00
,0x00,0x30,0x18,0x0c,0x0c,0x00,0x00
,0x00,0x0c,0x64,0x36,0x18,0x00,0x00
,0x00,0x0f,0x83,0xc1,0xf8,0x00,0x00
,0x00,0x0f,0x81,0xc1,0xf8,0x00,0x00
,0x00,0x0c,0x60,0x0c,0x18,0x00,0x00
,0x00,0x0c,0x70,0x0e,0x18,0x00,0x00
,0x00,0x0c,0x78,0x0e,0x18,0x00,0x00
,0x00,0x0c,0x00,0x00,0x18,0x00,0x00
,0x00,0x06,0x07,0xf0,0x60,0x00,0x00
,0x00,0x06,0x07,0xf0,0x60,0x00,0x00
,0x00,0x01,0x9f,0xfd,0x80,0x00,0x00
,0x00,0x63,0x9f,0xf0,0xe7,0x00,0x00
,0x00,0xf6,0x07,0xf0,0x67,0x00,0x00
,0x00,0xdc,0x03,0xc0,0x1b,0x00,0x00
,0x00,0xdc,0x01,0xc0,0x1b,0x00,0x00
,0x00,0xdc,0x00,0x00,0x1b,0x00,0x00
,0x00,0xc7,0x87,0xf1,0xe3,0x00,0x00
,0x00,0xc3,0xff,0xff,0xe3,0x00,0x00
,0x00,0xc0,0x78,0x0e,0x03,0x00,0x00
,0x00,0x30,0x00,0x00,0x06,0x00,0x00
,0x00,0x30,0x00,0x00,0x06,0x00,0x00
,0x00,0x30,0x00,0x00,0x06,0x00,0x00
,0x00,0x0c,0x00,0x00,0x18,0x00,0x00
,0x00,0x0f,0x80,0x00,0x10,0x00,0x00
,0x00,0x07,0x80,0x01,0xe0,0x00,0x00
,0x00,0x00,0x7f,0xfe,0x00,0x00,0x00
,0x00,0x00,0x7f,0xfe,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM duck_2_bmp[] =
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

static const unsigned char* duck_img(){
  switch(growth){
    case 0: return duck_0_bmp;
    case 1: return duck_1_bmp;
    case 2: return duck_2_bmp;
    default: break;
  }
  return duck_2_bmp;
}

#define GLST
#endif
