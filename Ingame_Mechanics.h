#ifndef GAMEM
#include <Arduino_LSM9DS1.h>
#include <vector>
#include "Global_State.h"

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

int today_steps;
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
elapsed_time step_clock = elapsed_time(500);
float step_thrshld = 0.6;
float xavg, yavg, zavg;
bool st_flag = false;


void step_reset(){
    step_counts.push_back(today_steps);
    today_steps = 0;
}


void LSM9DS1_setup(){
//  while (!Serial);
//  Serial.println("Started");

  if (!IMU.begin()) {
     Serial.println("Failed to initialize IMU!");
    while (1);
  }

   Serial.print("Accelerometer sample rate = ");
   Serial.print(IMU.accelerationSampleRate());
   Serial.println("Hz"); 
}

void calibrate(){
  float xval, yval, zval;
  
  for (int i = 0; i < 100; i++) {
    if (IMU.accelerationAvailable()) IMU.readAcceleration(xval, yval, zval);
    xavg+= xval; yavg+= yval; zavg+= zval;
  }
  
  delay(100);
  xavg /= 100.0; yavg /= 100.0; zavg /= 100.0;
  Serial.println(xavg); Serial.println(yavg); Serial.println(zavg);
}

void step_count(){
    float totvect = 0, prev_totvect = 0;
    float totave = 0;
    float xaccl = 0, yaccl = 0, zaccl = 0;

    if (IMU.accelerationAvailable()) IMU.readAcceleration(xaccl, yaccl, zaccl);
    xaccl-=xavg; yaccl-=yavg; zaccl-=zavg;
    
    totvect = sqrt(xaccl*xaccl + yaccl*yaccl + zaccl*zaccl);
    totave = (totvect + prev_totvect) / 2 ;
    prev_totvect = totvect;

//    Serial.println("totave"); Serial.println(totave);

    if(totave>step_thrshld && !st_flag){
        today_steps++;
        st_flag = true;
    }

    if(totave<step_thrshld && st_flag) st_flag = false;
    if(today_steps<0) today_steps = 0;

//    Serial.println('\n'); Serial.print("steps: "); Serial.println(steps);
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
    return st_flag;
}

#define GAMEM
#endif
