#include <ArduinoSTL.h>
#include "Kiduck_utility.h"
#include "Ingame_Mechanics.h"

#ifndef APPSNC

#define COMM_INFO 0
#define COMM_GROWTH 1
#define COMM_NAME 2


int setupAppBlueToothConnection();

int sendString(String sendVal);
int sendKiDuckInfo();
int sendFunc(int c);

int parseName();
int parseGrowth();
int parseFunc(int c);

void send(int c);
void recv(int c);


void appBleSetup(){
  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!!");
  setupAppBlueToothConnection(); //initialize Bluetooth
  //this block is waiting for connection was established.
  while (1){
    delay(200);
    if (recvMsg(100)!=0) continue;
    if (recvcmp("OK+CONN")!=0) continue;
    break;
  }
  Serial.println("connected\r\n");
}

bool syncApp(){
  delay(200);
  if (recvMsg(1000) == 0){
    if      (recvcmp("AppConnected")==0)    send(COMM_INFO);
    else if (recvcmp("InitGrowth")==0)      send(COMM_GROWTH);
    else if (recvcmp("SetGrowth")==0)       recv(COMM_GROWTH);
    else if (recvcmp("InitName")==0)        send(COMM_NAME);
    else if (recvcmp("SetName")==0)         recv(COMM_NAME);
    else if (recvcmp("OK+LOST")==0)         return false;

    Serial.print("recv: ");
    Serial.println((char *)recv_str);
  }
  return true;
}


void send(int c){
    if (sendFunc(c) == -1) 
        blueToothSerial.print("Fail to send        ");
}

void recv(int c){
    blueToothSerial.print("ACK");
    if (recvMsg(1000)) return;
    if (parseFunc(c)==0)
        blueToothSerial.print("SUCCESS");
    else
        blueToothSerial.print("FAIL");
}


//configure the Bluetooth through AT commands
int setupAppBlueToothConnection(){
  Serial.print("Setting up Bluetooth link\r\n");
  delay(2000);//wait for module restart
  blueToothSerial.begin(9600);

  //wait until Bluetooth was found
  while (1){
    delay(500);
    if (sendBlueToothCommand("AT") != 0) continue;
    if (recvcmp("OK") == 0) break;
  }
  Serial.println("Bluetooth exists\r\n");

  //configure the Bluetooth
  sendBlueToothCommand("AT+RENEW");//restore factory configurations
  delay(2000);
  sendBlueToothCommand("AT+NAME?");
  sendBlueToothCommand("AT+NAMEKIDUCK");
  sendBlueToothCommand("AT+ROLE0");//set to slave mode
  sendBlueToothCommand("AT+RESTART");//restart module to take effect
  delay(2000);//wait for module restart

  //check if the Bluetooth always exists
  if (sendBlueToothCommand("AT") == 0){
    if (recvcmp("OK") == 0){
      Serial.print("Setup complete\r\n\r\n");
      return 0;
    }
  }
  return -1;
}


//########################## Send ##########################//

int sendString(String sendVal){
  sendVal = fixLen(sendVal);
  if(!sendVal) return -1;
  
  blueToothSerial.print(sendVal);
  return 0;
}

int sendKiDuckInfo() {
  String sendName = fixLen(user_name);
  if(!sendName) return -1;

  String sendDataCount = fixLen(String(dataCount));
  if(!sendDataCount) return -1;

  std::vector<String> sendData(dataCount);
  for (int i = 0; i < dataCount; i++) {
    sendData[i] = fixLen(strJoin(data[i][0], data[i][1], data[i][2]));
    if(!sendData[i]) return -1;
  }

  blueToothSerial.print(sendName);
  blueToothSerial.print(sendDataCount);
  for (int i = 0; i < dataCount; i++) {
    blueToothSerial.print(sendData[i]);
  }
  return 0;
}

int sendFunc(int c){
    switch(c){
        case COMM_INFO:   return sendKiDuckInfo();
        case COMM_GROWTH: return sendString(strJoin(threshold[0], threshold[1], threshold[2]));
        case COMM_NAME:   return sendString(user_name);
        default: return -1;
    }
}


//########################## Parse ##########################//
int parseGrowth() {
  String strStepCriteria = "";
  String strDrinkCriteria = "";
  String strCommCriteria = "";

  int type = 0; // 0: step, 1: drink, 2: communication
  for (int curi = 0; recv_str[curi]!='\0';curi++) {
    if (recv_str[curi] == ' ') type++;
    else if (recv_str[curi] >= '0' && recv_str[curi] <= '9') {
        switch(type){
            case 0:  strStepCriteria += recv_str[curi]; break;
            case 1:  strDrinkCriteria += recv_str[curi]; break;
            default: strCommCriteria += recv_str[curi]; break;
        }
    }else return -1;
  }
  Serial.println("update criteria: " + strJoin(strStepCriteria, strDrinkCriteria, strCommCriteria));

  threshold[0] = strStepCriteria.toInt();
  threshold[1] = strDrinkCriteria.toInt();
  threshold[2] = strCommCriteria.toInt();

  return 0;
}

int parseName(){
  String newName = "";
  newName = String((char *)recv_str);
  Serial.println(newName);
  user_name = newName;
  return 0;
}


int parseFunc(int c){
    switch(c){
        case COMM_GROWTH: return parseGrowth();
        case COMM_NAME:   return parseName();
        default: return -1;
    }
}


#define APPSNC
#endif