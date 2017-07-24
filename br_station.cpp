#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

char ssid[32] = {0,};
char pass[32] = {0,};
char dev_id[32] = {0,};
char dev_pass[32] = {0,};
char gw_id[32] = {0,};
char e_reset[32] = {0,};


int br_station_read_eeprom(void)
{
#if 1  
  for (int i = 0; i < 32; i++) { ssid[i] = (char) EEPROM.read(i); }
  for (int i = 0; i < 32; i++) { pass[i] = (char) EEPROM.read(i + 32); }
  for (int i = 0; i < 32; i++) { e_reset[i] = (char) EEPROM.read(i + 64); }
 
  Serial.println("read eeprom");
  Serial.print("ssid : "); Serial.println(ssid);
  Serial.print("pass : "); Serial.println(pass);
  Serial.print("e_reset : "); Serial.println(e_reset);
#else
  //do nothing
#endif
  return 0;
}

