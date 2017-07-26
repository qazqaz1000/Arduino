#include <ESP8266WiFi.h>
#include "tcpServer.h"
#include <ArduinoJson.h>
#include <EEPROM.h>

const char *ap_ssid2 = "esp8266";
const char *ap_pass2 = "12345678";
 WiFiServer server3(2378);
WiFiClient serverClients[MAX_SRV_CLIENTS];
IPAddress ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);


void testserverSetup(){
//    WiFi.softAP(ap_ssid2, ap_pass2,6,0);
  WiFi.softAP(ap_ssid2, ap_pass2);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1),IPAddress(192, 168, 4, 1),IPAddress(255, 255, 255, 0));
    
  Serial.print(F("\nConnecting to ")); Serial.println(ap_ssid2);
  Serial.print(F("server started.. "));
  Serial.print(F("ip: ")); Serial.println(WiFi.softAPIP());
  server3.begin();
}


int testcheckClient() {
    uint8_t i;
    String json="";               //do parsing
      int isOk=0;
    //check if there are any new clients
    if (server3.hasClient()){      //if has client
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server3.available();
          continue;
          }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server3.available();
    serverClient.stop();
    }
    //check clients for data
    
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
          while(serverClients[i].available()) {
              char inchar = serverClients[i].read();  
              json+=inchar;
          }
          Serial.print("json = ");
          Serial.println(json);
          StaticJsonBuffer<400> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(json);
          if (!root.success()) {
            Serial.println("parseObject() failed");
            return 0;
          }
          //clear eeprom
          uint8_t blank=0;
          for (int i = 0; i < 32; i++) { EEPROM.write( 0 + i, blank); }    
          for (int i = 0; i < 32; i++) { EEPROM.write(32 + i, blank); }
          for (int i = 0; i < 32; i++) { EEPROM.write(64 + i, blank); }
          
          EEPROM.commit();
          
          String p_ssid = root["ssid"];
          String p_pass = root["pass"];
          String p_isReset = "false";
          Serial.println(p_ssid);
          Serial.println(p_pass);
          for (int i = 0; i < p_ssid.length(); i++) { EEPROM.write( 0 + i, p_ssid[i]); }    
          for (int i = 0; i < p_pass.length(); i++) { EEPROM.write(32 + i, p_pass[i]); }
          for (int i = 0; i < p_isReset.length(); i++) { EEPROM.write(64 + i, p_isReset[i]); }
          isOk = EEPROM.commit();
          Serial.println(isOk);
//          if(isOk==1) serverClients[i].write("\{\"response\":\"success\"\}");
//          else serverClients[i].write("\{\"response\":\"fail\"\}");
//          serverClients[i].clear();
//          serverClients[i].clear();
        }
      }
    }
    if(isOk) return 1;
    else    return 0;
      
}
void closeServer(){
  Serial.println("close");
  WiFi.softAPdisconnect(true);
  server3.close();
}

void send_client_message(int index){
  int i=0;
  
  switch(index){
    case 0:
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
          serverClients[i].write("FAIL_WIFI_CONNECTED");
      }
    break;
    case 1:
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
          serverClients[i].write("FAIL_DEVICE_CONNECTED");
    }
    break;
    case 2:
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
          serverClients[i].write("FAIL_SEND_TAGSTREAM");
    }
    break;
    case 3:
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
          serverClients[i].write("SUCCESS_CONNECT");
    }
    break;
  }
}

//WiFiClient time_client;
//String getTime() {
//  while (!time_client.connect("naver.com", 80)) {}
//  time_client.print("HEAD / HTTP/1.1\r\n\r\n");
//  while(!time_client.available()) {}
//  
//  while(time_client.available()){
//    if (time_client.read() == '\n') {    
//      if (time_client.read() == 'D') {    
//        if (time_client.read() == 'a') {    
//          if (time_client.read() == 't') {    
//            if (time_client.read() == 'e') {    
//              if (time_client.read() == ':') {    
//                time_client.read();
//                String timeData = time_client.readStringUntil('\r');
//                time_client.stop();
//                return timeData;
//              }
//            }
//          }
//        }
//      }
//    }
//  }
//} 
