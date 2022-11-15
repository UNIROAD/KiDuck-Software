#include <SoftwareSerial.h>   //Software Serial Port
#include <ArduinoSTL.h>
#include "Kiduck_utility.h"

#ifndef BLE

#define RxD 4
#define TxD 5

#define COMM_INFO 0
#define COMM_GROWTH 1
#define COMM_NAME 2

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

//########################## string ##########################//

//compare given string with recv_str
int recvcmp(const char *str) {return strcmp((char *)recv_str, str);}

int strcmp(char *a, const char *b){
  for(unsigned int ptr = 0; a[ptr] != '\0'; ptr++){
    if (a[ptr] != b[ptr]) return -1;
  }
  return 0;
}

//########################## Bluetooth ##########################//

//receive message from Bluetooth with time out
int recvMsg(unsigned int timeout){
  //wait for feedback
  unsigned char num;
  unsigned char len = 100;
  int i = 0;

  //waiting for the first character with time out
  for(unsigned int time = 0;!blueToothSerial.available();time++){
    delay(50);
    if (time > (timeout / 50)) return -1;
  }

  //read other characters from uart buffer to string
  for(;blueToothSerial.available() && (i < 100);i++)
    recv_str[i] = char(blueToothSerial.read());

  recv_str[i] = '\0';

  return 0;
}

//send command to Bluetooth and return if there is a response received
int sendBlueToothCommand(char command[]){
  Serial.print("send: ");
  Serial.println(command);

  blueToothSerial.print(command);
  delay(300);

  if (recvMsg(200) != 0) return -1;

  Serial.print("recv: ");
  Serial.println(recv_str);
  return 0;
}


//configure the Bluetooth through AT commands
int setupBlueToothConnection(){
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

String strJoin(String str1, String str2, String str3){
  return str1 + " " + str2 + " " + str3;
}
String strJoin(int str1, int str2, int str3){
  return String(str1) + " " + String(str2) + " " + String(str3);
}

String fixLen(String sendVal){
  if (sendVal.length() > 20)
    return "";

  while (sendVal.length() < 20) {
    sendVal += ' ';
  }
  return sendVal;
}

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

int parseName() {
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


//########################## Bluetooth ##########################//
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


void bleSetup(){
  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!!");
  setupBlueToothConnection(); //initialize Bluetooth
  //this block is waiting for connection was established.
  while (1){
    delay(200);
    if (recvMsg(100)!= 0) continue;
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

#define BLE
#endif