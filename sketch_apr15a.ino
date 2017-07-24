
//#undef ARDUINO

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <FirebaseArduino.h>
#include <time.h>

//#include <SoftwareSerial.h>


#include "br_ble.h"
#include "br_station.h"
#include "tcpServer.h"

//#define FIREBASE_HOST "irisysiottest01.firebaseio.com"
//#define FIREBASE_AUTH "fllBnKuGbbtSdd7YwvGV1V4sOIeFvfJ4dSNTr4KR"
#define FIREBASE_HOST "fordevelop-94a7d.firebaseio.com"
#define FIREBASE_AUTH "6QtEeFl8GXs8ofroHJTcR5VanmKfrD9yTvOPDCLc"
extern char ssid[32];
extern char pass[32];

void checkReset();
void actSTA();
void connectWifi();
void firebaseSetup();
void sendFirebase(int index);
void checkFirebase();
String getTime();
void flipD1();
void flipD2();
void flipD1D2();

String tagString[10];
String receive_MAC = "";

SoftwareSerial swSerial(14, 12, false, 256); //for serial print

char e_isReset[32]={0,};
int isReset=0;
int resetCount=0;       //reset pushed count
int checkClient=0;
String f_message = "";
String f_timestamp_past= "";
String f_timestamp_curr= "";
String f_type = "";
String f_userid = "";
String f_mac = "";

const String path_MAC = "/userlogs/bridgeid/registered/device";
void setup() {
  EEPROM.begin(512);
  
  delay(10);

  pinMode(D1, OUTPUT);  //blue LED
  pinMode(D2, OUTPUT);   //green LED
  pinMode(D7, INPUT);  //switching

  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);

   
  Serial.begin(9600);
  swSerial.begin(9600);
  
  Ticker flipper;
  flipper.attach(0.2, flipD1);


  wdt_enable(5);
//  ESP.wdtDisable();
  while (Serial.available() > 0) {
    Serial.read(); //flush buffer
  }

  delay(100);
  checkReset();
  for (int i = 0; i < 32; i++) { e_isReset[i] = (char) EEPROM.read(i + 64); }
  isReset = strcmp(e_isReset, "false");
//  Serial.println(isReset);
  if(isReset!=0){
    Serial.println("AP AND STA");
    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    testserverSetup();
  }
  else{
    Serial.println("STA");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(50);
    br_station_read_eeprom();

  }

  flipper.detach();
 digitalWrite(D1, HIGH);
  Serial.println(F("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"));

//  String text = "REQ:S:OPEN,0123456789AB";
//  Serial.print(text.find(','));
// while(Serial.available()) {
//    char inchar = Serial.read();
//    text+=inchar;
//    index_Enter++;
//    if(inchar == ','){  // find ',' 
//      index_Comma = index_Enter;
//    }
//    
//    if(inchar == '\n'){
//      sendIOT = true;
//      break;
//    }
//  }
  
}

//**************************************************************************//
//******************************* L O O P **********************************//
//**************************************************************************//

int a=0;
void loop() {

  Ticker flipper;
  //if first enroll AP+STA
  if(isReset!=0){     
    checkClient = testcheckClient();
    if(checkClient){      
      flipper.attach(0.2, flipD1);
      br_station_read_eeprom();
      connectWifi();
      isReset=0;                    // escape
      sendFirebase(7);
      flipper.detach();
      digitalWrite(D1, HIGH);
      delay(100);
      closeServer();
    }    
  }
  
  else{  //if second STA
      if(WiFi.status() != WL_CONNECTED){  
        flipper.attach(0.2, flipD1);   
        connectWifi();
        flipper.detach();
        digitalWrite(D1, HIGH); 
      }
      sendFirebase(brBleMsgParse_MAC(&receive_MAC));
      
      static unsigned long tick = millis();
      if ( ( millis() - tick) > 100 )
      {
//        String aa = Firebase.getString("/userlogs/MAC/MAC1");
//        Serial.println(aa);
        checkFirebase();
        tick = millis();
//        time_t now = time("mm:ss");
//        Serial.println(ctime(&now));
      }
      
  }


 
  checkReset();  

  delay(10);
}

void connectWifi(){
  int wificount=0;
  WiFi.disconnect();
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED) {
    wificount++;
    Serial.print(".");
    checkReset();
    delay(500);
    if(wificount>40){
      Serial.println("\n wifi connect fail");
      WiFi.disconnect();
      WiFi.begin(ssid, pass);
      wificount=0;
    }
  }
  
  Serial.println("wifi connect success !");
  firebaseSetup();
  f_timestamp_past = Firebase.getString("/userlogs/bridgeid/lastlog/timestamp");
  Serial.println(f_timestamp_past);

  
//  /* time  */
//  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");
//  Serial.println("\nWaiting for time");
//  while (!time(nullptr)) {
//    Serial.print(".");
//    delay(1000);
//  }
//  Serial.println("");
}


void checkReset(){
  while(digitalRead(D7)){
    resetCount++;
    delay(100);
      if(resetCount>30){
          Serial.println("reset");
          String p_true = "true";
          uint8_t blank=0;
          Ticker flipper;
          flipper.attach(0.2, flipD1D2);
          for (int i = 0; i < 32; i++) { EEPROM.write( 0 + i, blank); }    
          for (int i = 0; i < 32; i++) { EEPROM.write(32 + i, blank); }
          for (int i = 0; i < 32; i++) { EEPROM.write(64 + i, blank); } 
          for (int i = 0; i < p_true.length(); i++) { EEPROM.write(64 + i, p_true[i]); }
          EEPROM.commit();
          delay(4000);
          flipper.detach();
      }
  }
  resetCount=0;  
}

void firebaseSetup(){  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);    
}

void sendFirebase(int index){
  String message;
  switch(index){
    case 0:
      message = BLE_MSG_S_RES_OPENOK;
    break;
    case 1:
      message = BLE_MSG_S_ALM_OPENED;
    break;
    case 2:
      message = BLE_MSG_S_ALM_CLOSED;
    break;
    case 3:
      message = BLE_MSG_S_ALM_LONGOPENED;     
    break;
    case 4:
      message = BLE_MSG_S_RES_OPENFAIL;
    break;
    case 5:
      message = BLE_MSG_S_RES_POWERON;
    break;
    case 6:
      message = BLE_MSG_S_RES_POWEROFF;
    break;

    
    case 10:
      message = BLE_MSG_D_RES_OPENOK;
    break;
    case 11:
      message = BLE_MSG_D_ALM_OPENED;     
    break;
    case 12:
      message = BLE_MSG_D_ALM_CLOSED;
    break;
    case 13:
      message = BLE_MSG_D_ALM_LONGOPENED;
    break;
    case 14:
      message = BLE_MSG_D_RES_OPENFAIL;
    break;
    
  }

  if(index<19){   
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"] = message;
    root["timestamp"] = getTime();
    root["userid"] = "namcheol";
//    Serial.println(receive_MAC);
    String macid = String(receive_MAC);
    String name = Firebase.push("/devicelog/bridgeid/"+macid, root );

    digitalWrite(D2, HIGH);
    delay(200);
    digitalWrite(D2, LOW); 
    if (Firebase.failed()) {
      Serial.print("\nFirebase Pushing  failed:");
      Serial.println(Firebase.error()); 
  
    }
  }
}

void checkFirebase(){

  f_timestamp_curr= Firebase.getString("/userlogs/bridgeid/lastlog/timestamp");
  if(f_timestamp_curr==""){
//    Serial.println("return");
    return;
  }
  if(f_timestamp_past != f_timestamp_curr){
    Ticker flipper;
    f_type = Firebase.getString("/userlogs/bridgeid/lastlog/message");
    f_mac = Firebase.getString("/userlogs/bridgeid/lastlog/mac");
//    f_mac = f_mac.substring(1,13);

    Serial.println(f_timestamp_curr);
    
    f_timestamp_past = f_timestamp_curr;
    
    if(f_type == "REQ:S:OPEN"){
      flipper.attach(0.2, flipD2);      
      f_type += ",";   //  
      f_type += f_mac;   //  
      f_type += "\n";   //  

      Serial.print("send : ");
      Serial.print(f_type);
      swSerial.print(f_type);
      delay(2500);
      flipper.detach();
    }
    
    else if(f_type == "REQ:D:OPEN"){
      flipper.attach(0.2, flipD2);
      f_type += ",";   //  
      f_type += f_mac;   //  
      f_type += "\n";   //  
      Serial.print("send : ");
      Serial.print(f_type);
      swSerial.print(f_type);
      delay(2500);
      flipper.detach();
    }
    
    else if(f_type.substring(0,3) == "ENR"  ){
      Serial.print("send : ");
      Serial.println(f_type);      
      uint8_t blank=0;
      char temp[16] = {0, };
      digitalWrite(D2, HIGH);      
      for (int i = 0; i < 16*10; i++) { EEPROM.write(96 + i, blank); }
      for(int i =0; i<10; i++){
        String n = String(i);
        String mac = Firebase.getString(path_MAC+n+"/mac");  //+macID        
        if(mac =="") 
          break;
        Serial.println(n+" : "+mac);                
        for (int j = 0; j < mac.length(); j++) { EEPROM.write(96 + j + (16*i), mac[j]); }
        EEPROM.commit();        
      }
      for(int i = 0; i < 10; i++){
        for (int j = 0; j < 16; j++) { temp[j] = (char) EEPROM.read(96+ j + (16*i)); }
//        Serial.println(temp);
      }
      if(f_type.substring(4,8) == "SAFE"){
        String req = "";
        req = "REQ:S:OPEN,";
        req += f_mac;
        req += "\n";
        Serial.print(req);
        swSerial.print(req);
      }
      delay(2000);
      digitalWrite(D2, LOW);
    }
    
    else if(f_type == "DEL:MAC"){
      Serial.print("send : ");
      Serial.println(f_type); 
      char temp[16] = {0, }; 
      uint8_t blank=0;
      digitalWrite(D2, HIGH);

      for(int i = 0; i < 10; i++){
        for (int j = 0; j < 16; j++) { temp[j] = (char) EEPROM.read(96+ j + (16*i)); }
        if(String(temp) == f_mac){
          Serial.println("DELETE MAC : " + f_mac);
          for (int j = 0; j < 16; j++) { EEPROM.write(96 + j + (16*i), blank); }
          EEPROM.commit();
        }
      }
      delay(2000);
      digitalWrite(D2, LOW);
    }
    
  }
}

WiFiClient time_client;
String getTime() {
  while (!time_client.connect("google.com", 80)) {}
  time_client.print("HEAD / HTTP/1.1\r\n\r\n");
  while(!time_client.available()) {}
  
  while(time_client.available()){
    if (time_client.read() == '\n') {    
      if (time_client.read() == 'D') {    
        if (time_client.read() == 'a') {    
          if (time_client.read() == 't') {    
            if (time_client.read() == 'e') {    
              if (time_client.read() == ':') {    
                time_client.read();
                String timeData = time_client.readStringUntil('\r');
                time_client.stop();
                return timeData;
              }
            }
          }
        }
      }
    }
  }
} 

void flipD1()
{
  int state = digitalRead(D1);  // get the current state of GPIO1 pin
  digitalWrite(D1, !state);     // set pin to the opposite state

}

void flipD2()
{
  int state = digitalRead(D2);  // get the current state of GPIO1 piot
  digitalWrite(D2, !state);     // set pin to the opposite state

}

void flipD1D2()
{
  int state1 = digitalRead(D1);  // get the current state of GPIO1 pin
  digitalWrite(D1, !state1);     // set pin to the opposite state
  int state2 = digitalRead(D2);  // get the current state of GPIO1 pin
  digitalWrite(D2, !state2);     // set pin to the opposite state

}
