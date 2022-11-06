#include <SoftwareSerial.h>   //Software Serial Port
#include <ArduinoSTL.h>

#include "Global_State.h"

#ifndef BLE

#define RxD 4
#define TxD 5

SoftwareSerial blueToothSerial(RxD, TxD); //the software serial port

char recv_str[100];

int dataCount = 8;
int data[8][3] = { // first index: day, second index: 0-step count 1-drink 2-communication
  {2991, 2000, 6},
  {12444, 2314, 8},
  {7004, 1599, 5},
  {8932, 1211, 2},
  {12322, 1522, 4},
  {4052, 2344, 2},
  {9806, 1112, 9},
  {10000, 1234, 5}
};

int threshold[3] = {10000, 1500, 5};
String password = "abcdefg";
int emergencyAlarm = 0;

int escape_flag = 0;

#define COMM_INFO 0
#define COMM_GROWTH 1
#define COMM_NAME 2

//########################## string ##########################//
//used for compare two string, return 0 if one equals to each other
int strcmp(char *a, char *b){
  for(unsigned int ptr = 0; a[ptr] != '\0'; ptr++){
    if (a[ptr] != b[ptr]) return -1;
  }
  return 0;
}

//compare given string with recv_str
int recvcmp(char *str) {return strcmp((char *)recv_str, str);}

int strcmp2(char *a, const char *b){
  for(unsigned int ptr = 0; a[ptr] != '\0'; ptr++){
    if (a[ptr] != b[ptr]) return -1;
  }
  return 0;
}

//########################## Bluetooth ##########################//

//receive message from Bluetooth with time out
int recvMsg(unsigned int timeout)
{
  //wait for feedback
  unsigned int time = 0;
  unsigned char num;
  unsigned char i;

  //waiting for the first character with time out
  i = 0;
  while (1){
    delay(50);
    if (blueToothSerial.available()){
      recv_str[i] = char(blueToothSerial.read());
      i++;
      break;
    }
    time++;
    if (time > (timeout / 50)) return -1;
  }

  //read other characters from uart buffer to string
  for(;blueToothSerial.available() && (i < 100);i++)
    recv_str[i] = char(blueToothSerial.read());

  recv_str[i] = '\0';

  return 0;
}

//send command to Bluetooth and return if there is a response received
int sendBlueToothCommand(char command[])
{
  Serial.print("send: ");
  Serial.print(command);
  Serial.println("");

  blueToothSerial.print(command);
  delay(300);

  if (recvMsg(200) != 0) return -1;

  Serial.print("recv: ");
  Serial.print(recv_str);
  Serial.println("");
  return 0;
}


//configure the Bluetooth through AT commands
int setupBlueToothConnection()
{
  Serial.print("Setting up Bluetooth link\r\n");
  delay(2000);//wait for module restart
  blueToothSerial.begin(9600);

  //wait until Bluetooth was found
  while (1){
    if (sendBlueToothCommand("AT") == 0){
      if (recvcmp((char *)"OK") == 0){
        Serial.println("Bluetooth exists\r\n");
        break;
      }
    }
    delay(500);
  }

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
    if (recvcmp((char *)"OK") == 0){
      Serial.print("Setup complete\r\n\r\n");
      return 0;
    }
  }
  return -1;
}


//########################## Send ##########################//
int sendKiDuckInfo() {
  String sendName = user_name;
  if (sendName.length() > 20)
    return -1;

  while (sendName.length() < 20) {
    sendName += ' ';
  }

  String sendDataCount(dataCount);
  if (sendDataCount.length() > 20)
    return -1;

  while (sendDataCount.length() < 20) {
    sendDataCount += ' ';
  }

  std::vector<String> sendData(dataCount);
  for (int i = 0; i < dataCount; i++) {
    sendData[i] = String(data[i][0]) + ' ' + String(data[i][1]) + ' ' + String(data[i][2]);
    if (sendData[i].length() > 20)
      return -1;

    while (sendData[i].length() < 20) {
      sendData[i] += ' ';
    }
  }

  blueToothSerial.print(sendName);
  blueToothSerial.print(sendDataCount);
  for (int i = 0; i < dataCount; i++) {
    blueToothSerial.print(sendData[i]);
  }
  return 0;
}

int sendKiDuckGrowth() {
  String sendThreshold = String(threshold[0]) + ' ' + String(threshold[1]) + ' ' + String(threshold[2]);
  if (sendThreshold.length() > 20)
    return -1;

  while (sendThreshold.length() < 20) {
    sendThreshold += ' ';
  }

  blueToothSerial.print(sendThreshold);
  return 0;
}

int senduser_name() {
  String sendName = user_name;
  if (sendName.length() > 20)
    return -1;

  while (sendName.length() < 20) {
    sendName += ' ';
  }

  blueToothSerial.print(sendName);
  return 0;
}

int sendFunc(int c){
    switch(c){
        case 0: return sendKiDuckInfo();
        case 1: return sendKiDuckGrowth();
        case 2: return senduser_name();
        default: return -1;
    }
}


//########################## Parse ##########################//
int parseGrowth() {
  String strStepCriteria = "";
  String strDrinkCriteria = "";
  String strCommCriteria = "";

  int type = 0; // 0: step, 1: drink, 2: communication
  for (int curi = 0;recv_str[curi] != '\0';curi++) {
    if (recv_str[curi] == ' ') type++;
    else if (recv_str[curi] >= '0' && recv_str[curi] <= '9') {
        switch(type){
            case 0: strStepCriteria += recv_str[curi]; break;
            case 1: strDrinkCriteria += recv_str[curi]; break;
            default: strCommCriteria += recv_str[curi]; break;
        }
    }else return -1;
  }
  Serial.println("update criteria:");
  Serial.println(strStepCriteria);
  Serial.println(strDrinkCriteria);
  Serial.println(strCommCriteria);

  threshold[0] = strStepCriteria.toInt();
  threshold[1] = strDrinkCriteria.toInt();
  threshold[2] = strCommCriteria.toInt();

  return 0;
}

int parseName() {
  String newName = "";
  newName = String((char *)recv_str);
  Serial.println(newName);
  user_name = newName;
  return 0;
}


int parseFunc(int c){
    switch(c){
        case 1: return parseGrowth();
        case 2: return parseName();
        default: return -1;
    }
}

void send(int c){
    if (sendFunc(c) == -1) 
        blueToothSerial.print("Fail to send        ");
}

void recv(int c){
    blueToothSerial.print("ACK");
    if (recvMsg(1000)) return;
    if (parseFunc(c) == 0)
        blueToothSerial.print("SUCCESS");
    else
        blueToothSerial.print("FAIL");
}


void bleSetup()
{
  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!!");
  setupBlueToothConnection(); //initialize Bluetooth
  //this block is waiting for connection was established.
  while (1){
    if (recvMsg(100) == 0){
      if (recvcmp((char *)"OK+CONN")==0){
        Serial.println("connected\r\n");
        break;
      }
    }
    delay(200);
  }
}

void syncApp(){
  delay(200);
  if (recvMsg(1000) == 0){
    if      (recvcmp((char *)"AppConnected")==0)    send(COMM_INFO);
    else if (recvcmp((char *)"InitGrowth")==0)      send(COMM_GROWTH);
    else if (recvcmp((char *)"SetGrowth")==0)       recv(COMM_GROWTH);
    else if (recvcmp((char *)"InitName")==0)        send(COMM_NAME);
    else if (recvcmp((char *)"SetName")==0)         recv(COMM_NAME);
    else if (recvcmp((char *)"OK+LOST")==0)         escape_flag = 1;

    Serial.print("recv: ");
    Serial.println((char *)recv_str);
  }
}

#define BLE
#endif