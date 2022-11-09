#include <Arduino_LSM9DS1.h>
#include <ArduinoSTL.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include "Global_State.h"

#ifndef GAMEM
using namespace std;

//############# Game Criterion #############//
const int WALK_DAILY_CRIT = 12000;
const int WALK_PT_CRIT = 500;
const int WALK_PT = 100;

const int MEET_DAILY_CRIT = 5;
const int MEET_PT = 100;

const int WATER_WT_DAILY_CRIT = 0.03;
const int WATER_PT = 100;


//############# Game Datas #############//
int points = 0;

int today_steps = 0;
int today_meet_count;
float today_water;

vector<int> step_counts;
vector<int> meet_count;
vector<float> water_drink;


//############# Game Points #############//
void update_points(){
    points+=today_steps/WALK_PT_CRIT 
        + today_meet_count*MEET_PT 
        + (today_water>WATER_WT_DAILY_CRIT*user_weight)?WATER_PT:0;
//    step_reset(); meet_reset(); water_reset();
}

//############# Step Counter #############//

MPU6050 accelgyro;
//설정 값들
#define WORKING_THRESHOLD 0.4
#define UPPER_THRESHOLD 1

elapsed_time step_clock = elapsed_time(20);
int16_t ax, ay, az, gx, gy, gz;
float xavg, yavg, zavg;
float xcurr, ycurr, zcurr;
float xprev, yprev, zprev;
float xcal, ycal, zcal;

bool st_flag = false;
bool st_event = false;


void step_calibrate(){
  float sumx=0;
  float sumy=0;
  float sumz=0;

  for(int i=0;i<100;i++){
    accelgyro.getMotion6(&ax,&ay,&az,&gz,&gy,&gz);
    sumx+=ax/16384.0; sumy+=ay/16384.0; sumz+=az/16384.0;
  }
  xavg=sumx/100.0; yavg=sumy/100.0; zavg=sumz/100.0;
}

void adjust_accelgyro(){
  Serial.println("Updating internal sensor offset...");
  accelgyro.setXAccelOffset(-950);
  accelgyro.setYAccelOffset(-5200);
  accelgyro.setZAccelOffset(-920);
  accelgyro.setXGyroOffset(20);
  accelgyro.setYGyroOffset(56);
  accelgyro.setZGyroOffset(-85);
}

void step_setup(){
  accelgyro.initialize();

  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

  //adjust_accelgyro();
  step_calibrate();

  xcurr = 0; ycurr = 0; zcurr = 0;
}

double dist;

void step_count(bool hasd){
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  xprev = xcurr; yprev = ycurr; zprev = zcurr;
  xcurr=ax/16384.0-xavg; ycurr=ay/16384.0-yavg; zcurr=az/16384.0-zavg;
  xcal=xcurr-xprev;ycal=ycurr-yprev;zcal=zcurr-zprev;

  dist = sqrt(xcal*xcal+ycal*ycal+zcal*zcal);
  double major_factor= max(xcurr,max(ycurr,zcurr));
  bool is_working = WORKING_THRESHOLD < dist && dist < UPPER_THRESHOLD;

  Serial.print("Variable_1:");Serial.print(dist);
  Serial.print(",Variable_2:");Serial.print(major_factor);
  Serial.print(",Variable_3:");Serial.print(WORKING_THRESHOLD);
  Serial.print(",Variable_4:");Serial.print(UPPER_THRESHOLD);
  Serial.print(",Variable_5:");Serial.println((int)(is_working&&st_flag == 0&&major_factor >0));
  
  st_event = is_working && !st_flag && major_factor>0;
  if(st_event){
    today_steps++;
    st_flag=true;
    Serial.print("step="); Serial.println(today_steps);
    delay(200);
  }
  if(!is_working && st_flag) st_flag=false;

  if(hasd) delay(20);
}


void step_reset(){
    step_counts.push_back(today_steps);
    today_steps = 0;
}


//############# Friend Meeting #############//
vector<int> today_friend;

void meet_reset(){
    if(today_friend.size()){
        meet_count.push_back(today_meet_count);
        today_friend.clear();
    }
    today_meet_count = 0;
}

bool is_not_in(int id){
    for(int i=0;i<(int)today_friend.size();i++){
        if(id==today_friend[i]) return false;
    }
    return true;
}

bool meet_add(int id){
    if(is_not_in(id)){
        today_friend.push_back(id);
        today_meet_count++;
        return true;
    }
    return false;
}



//############# Water Drinking #############//
float prev_water_level;

void water_reset(){
    water_drink.push_back(today_water);
    today_water = 0;
    prev_water_level = 0;
}

bool water_update(float curr_water_level){
    float diff = prev_water_level-curr_water_level;
    if(diff>0){
        today_water+=diff;
        return true;
    }
    return false;
}


bool change_event(){
    return st_event;
}

#define GAMEM
#endif
