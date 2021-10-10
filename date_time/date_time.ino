#include <WiFi.h>
#include "time.h"

const char* ssid     = "Schiele";
const char* password = "1234567898";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  localTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop(){
  delay(1000);
  localTime();
}

String localTime(){
  struct tm timeinfo;
  char current_time[19+1]; //19 chars + \n

  char year[4+1];
  char month[2+1];
  char day[2+1];
  char hours[2+1];
  char minutes[2+1];
  char seconds[2+1];

  if(!getLocalTime(&timeinfo)){
    (String("1900-") + random(01,12) + String("-") + random(1,28) + "T" + random(0,24) + ":" + random(0,59) + ":" + random(0,59)).toCharArray(current_time, sizeof(current_time));
    String("1900").toCharArray(year, sizeof(year));
    String(random(1,12)).toCharArray(month, sizeof(month));
    String(random(1,28)).toCharArray(day, sizeof(day));
    String(random(0,59)).toCharArray(hours, sizeof(hours));
    String(random(0,59)).toCharArray(minutes, sizeof(minutes));
    String(random(0,59)).toCharArray(seconds, sizeof(seconds));

    //Serial.println(String("1900-") + random(01,12) + String("-") + random(1,28) + "T" + random(0,24) + ":" + random(0,59) + ":" + random(0,59));
  }else{
    strftime(current_time, sizeof(current_time), "%Y-%B-%dT%H:%M:%S", &timeinfo);
    strftime(year, sizeof(current_time), "%Y", &timeinfo);
    strftime(month, sizeof(current_time), "%B", &timeinfo);
    strftime(day, sizeof(current_time), "%d", &timeinfo);
    strftime(hours, sizeof(current_time), "%H", &timeinfo);
    strftime(minutes, sizeof(current_time), "%M", &timeinfo);
    strftime(seconds, sizeof(current_time), "%S", &timeinfo);
  }

  Serial.println(current_time);
  return current_time;
}
