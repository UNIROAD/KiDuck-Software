#include <Arduino_LSM9DS1.h>

#ifndef GAMEM

const int WALK_DAILY_CRITERION = 3000;
const int WATER_DAILY_CRITERION = 10;

int water();
int walk();

void condition(){

}


// Step Counter
float step_thrshld = 0.6;
float xavg, yavg, zavg;
int steps;
bool st_flag = false;


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
        steps++;
        st_flag = true;
    }

    if(totave<step_thrshld && st_flag) st_flag = false;
    if(steps<0) steps = 0;

//    Serial.println('\n'); Serial.print("steps: "); Serial.println(steps);
}



bool change_event(){
    return st_flag;
}

#define GAMEM
#endif
