#include <ArduinoSTL.h>
#include "Kiduck_utility.h"
#include "Ingame_Mechanics.h"

#ifndef WTBT

// new functions to make a BLE connection with the bottle
int connectWithBottle(); // Make a BLE Connetion with the Kiduck bottle
int scanBottle(); // Scan BLE Device and Return the Index of the Kidcuk Bottle

// modified functions fit to the bottle
int setupBottleBlueToothConnection(); // init ble settings


// BLE setup for the connection with Bottle
void bottleBleSetup(){
  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!!");
  setupBottleBlueToothConnection(); //initialize Bluetooth
  //this block is waiting for connection was established.
  if(connectWithBottle() == 0)
    Serial.println("connected");
  else 
    Serial.println("fail to connect");
}

bool syncBottle() {
  if(recvMsg(200) == 0){
    if(recvcmp("OK+LOST") == 0) return false;
    Serial.println(recv_str);
    Serial.print("total: ");
    today_water += int(recv_str);
    Serial.println(today_water);
  }
  return true;
}


int setupBottleBlueToothConnection(){
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
  sendBlueToothCommand("AT+IMME1");
  sendBlueToothCommand("AT+NAMEKIDUCK");
  sendBlueToothCommand("AT+ROLE1");//set to slave mode
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

int connectWithBottle(){
  String connectMsg = "AT+CONN";
  String confirmMsg = "OK+CONN";
  
  int bottleIndex = scanBottle();
  if(bottleIndex < 0) return -1;

  connectMsg += String(bottleIndex);
  confirmMsg += String(bottleIndex) + confirmMsg;
  if(sendBlueToothCommand(connectMsg.c_str(), 3000) == 0){
    if(recvcmp(confirmMsg.c_str()) == 0){
      Serial.println("connected\r\n");
      return 0;
    }      
  }
  return -1;
}

int scanBottle(){
    String RECV_OK = "OK+";
    String SCAN_FINISH = "DISCE";
    String SCAN_NAME = "NAME:";
    String KIDUCK_BOTTLE = "KDB";
    int listCount = 0;
    int index = -1;
    if(sendBlueToothCommand("AT+DISC?", 3000) == 0) return index;
    if(recvcmp("OK+DISCS") == 0) return index;

    do{
        if(!cmpMsg(RECV_OK)) continue;
        Serial.println("OK");

        if(cmpMsg(SCAN_NAME)){
            Serial.println("NAME");
            listCount++;
            if(cmpMsg(KIDUCK_BOTTLE)) index = listCount - 1;        
        }
    }while(!cmpMsg(SCAN_FINISH, false));

    Serial.println("END");
    return index; 
}

#define WTBT
#endif