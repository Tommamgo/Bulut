

#include <ArduinoJson.h>
#include "SPIFFS.h"



void read_sp(){
    File file2 = SPIFFS.open("/data/export.json");
 
    if(!file2){
        Serial.println("Failed to open file for reading");
        return;
    }
 
    Serial.println("File Content:");
    Serial.println("Hallo ich möchste sterben"); 
 
    while(file2.available()){
 
        Serial.write(file2.read());
    }
 
    file2.close();
}

void spi_juliantot(){

  //StaticJsonDocument<200> doc;
  DynamicJsonDocument  doc(200);
  
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;

  JsonArray data = doc.createNestedArray("data");
  data.add(48.756080);
  data.add(2.302038);

  // Generate the minified JSON and send it to the Serial port.
  //serializeJson(doc, Serial);
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  // Start a new line
  Serial.println();
  serializeJsonPretty(doc, Serial);

  Serial.println("Hallo ich bin da, wo Bier"); 

    String sachen; 
  serializeJson(doc, sachen);

  //Serial.println(sachen); 
  delay(1000); 
  
  File file3 = SPIFFS.open("/data/export.json", FILE_APPEND);



  if (file3.print(sachen)) {
    Serial.println("File was written");
  } else {
    Serial.println("File write failed");
  }
 file3.close(); 

  
  
  
}

void write_sp(){
    //---------- Read file
    File fileToRead = SPIFFS.open("/data/export.json");
 
    if(!fileToRead){
        Serial.println("Failed to open file for reading");
        return;
    }
 
    Serial.println("File Content:");
 
    while(fileToRead.available()){
 
        Serial.write(fileToRead.read());
    }
 
    fileToRead.close();
}

void setup() {
  // Initialize Serial port
  Serial.begin(9600); 
  while (!Serial) continue;

  if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }

  



  
  // The above line prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
}

void loop() {
  read_sp(); 
  delay(1000); 
  spi_juliantot(); 
  delay(1000); 
}
