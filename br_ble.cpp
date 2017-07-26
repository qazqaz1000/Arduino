#include <Arduino.h>
#include <EEPROM.h>
#include "br_ble.h"
String text="";
String text_MAC="";
bool sendIOT=false;
int isApp=0;
//extern bool isEnroll;
//SoftwareSerial swSerial(14, 12, false, 256); //for serial print

int brBleMsgParse_MAC(String *receive_MAC)
{
  char msg_Protocol[32] = {0,};
  char msg_MAC[32] = {0,};

  static int index_Comma = 0;
  static int index_Enter = 0;
  int returnValue=19;
  static int ch=0;
  const int MAC_max=20;
  char e_MAC[MAC_max][32] = {0,};
  bool isFail = true;
//  Serial.println(isEnroll);
//
//  while(Serial.available()) {
//    char inchar = Serial.read();    
//    Serial.print(inchar);
//  }
//  return 19;
  
  while(Serial.available()) {
    char inchar = Serial.read();    
//    Serial.print(inchar);
    if(ch==0){
      text+=inchar;
      index_Comma++;
      if(inchar == ','){  // find ',' 
        ch = 1;
      }
    }
    else if(ch==1){
      text_MAC+=inchar;
      index_Enter++;
      if(inchar == '\n'){
        sendIOT = true;
        ch = 0;
        break;
      }
    }
    if(text == "v2.2\n"){
      Serial.println("version: "+text);
      index_Enter = 0;
      index_Comma = 0;
      text = "";
      text_MAC = "";
      memset(msg_Protocol, 0, 64);
      memset(msg_MAC, 0, 64);
      break;
    }
  }

  
  if(sendIOT){
    text.toCharArray(msg_Protocol, index_Comma);
    text_MAC.toCharArray(msg_MAC, index_Enter);
//    Serial.println("protocol : " + text);
//    Serial.println("mac : " + text_MAC);
    
    for(int i = 0; i < MAC_max; i++){
      for (int j = 0; j < 16; j++) { e_MAC[i][j] = (char) EEPROM.read(96+ j + (16*i)); }
//      Serial.println(e_MAC[i]);    
      if(!strcmp(e_MAC[i], msg_MAC)) {
        *receive_MAC = String(msg_MAC);
        isFail = false;
      }
    }
    if(isFail == true){
      Serial.print("none mac FAIL : ");
      Serial.println(msg_MAC);
      index_Enter = 0;
      index_Comma = 0;
      text = "";
      text_MAC = "";
      memset(msg_Protocol, 0, 64);
      memset(msg_MAC, 0, 64);
      sendIOT=false;
      return 19;
    }
    Serial.print(F(" received : "));
    if (!strncmp(msg_Protocol, BLE_MSG_S_RES_OPENOK, index_Comma)) {
      returnValue=0;
      isApp=1;  //app open
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_ALM_OPENED, index_Comma)) {
      if(!isApp){
        returnValue=1;
      }
      else{
        returnValue=19;
      }
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_ALM_CLOSED, index_Comma)) {
      returnValue=2;
      isApp=0;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_ALM_LONGOPENED, index_Comma)) {
      returnValue=3;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_RES_OPENFAIL, index_Comma)) {
      returnValue=4;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_RES_POWERON, index_Comma)) {
      returnValue=5;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_S_RES_POWEROFF, index_Comma)) {
      returnValue=6;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   도어락//       ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
    else if (!strncmp(msg_Protocol, BLE_MSG_D_RES_OPENOK, index_Comma)) {
      returnValue=10;
      isApp=1;  //app open
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_D_ALM_OPENED, index_Comma)) {
      if(!isApp){
        returnValue=11;
      }
      else{
        returnValue=19;
      }
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_D_ALM_CLOSED, index_Comma)) {
      returnValue=12;
      isApp=0;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_D_ALM_LONGOPENED, index_Comma)) {
      returnValue=13;
    }
    else if (!strncmp(msg_Protocol, BLE_MSG_D_RES_OPENFAIL, index_Comma)) {
      returnValue=14;
    }    
    else {
//      Serial.print(msg_Protocol);
//      Serial.print(",");
//      Serial.println(msg_MAC);
      Serial.println(F("  Unkown Message : "));

      sendIOT=false;
      returnValue=19;
    }
  
    Serial.print(msg_Protocol);
    Serial.print(",");
    Serial.println(msg_MAC);
    
    index_Enter = 0;
    index_Comma = 0;
    text = "";
    text_MAC = "";
    memset(msg_Protocol, 0, 64);
    memset(msg_MAC, 0, 64);

  }
  
  memset(msg_Protocol, 0, 64);
  memset(msg_MAC, 0, 64);
//  index_Enter = 0;
//  index_Comma = 0;
//  text = "";
//  text_MAC = "";
  
  sendIOT=false;
  return returnValue;
} // end brBleMsgParse



