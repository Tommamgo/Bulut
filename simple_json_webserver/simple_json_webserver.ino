// Import required libraries
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "NTPClient.h"
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Schiele";
const char* password = "1234567898";

// Set motor pin
const int ledPin = 2;
// Stores LED state
String ledState;

// Define an NTP Client to request data and time from an NTP server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// initialize String variables to save the date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Declaring array size
//const size_t array_capacity = JSON_ARRAY_SIZE(16) + JSON_OBJECT_SIZE(1) + 16*JSON_OBJECT_SIZE(9);
const size_t array_capacity = JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(4) + 16*JSON_OBJECT_SIZE(36);

// Create a preallocated memory pool to store a Json Object tree
// StaticJsonBuffer<200> jsonBuffer;
// DynamicJsonDocument array_doc(array_capacity);
StaticJsonDocument<array_capacity> array_doc;


// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState = "ON";
    }
    else {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void add2json(const String& line) {
  File file = SPIFFS.open("/export.json", FILE_WRITE);
  if (file.print(line)) {
    Serial.println("File was written");
  } else {
    Serial.println("File write failed");
  }
  file.close();
}


String getIt() {
  // Prepare JSON document
  DynamicJsonDocument doc(2048);
  doc["hello"] = "world";

  // Serialize JSON document
  String json;
  serializeJson(doc, json);

  return json;

}

String getJsonFromFile(){
  DynamicJsonDocument doc(2048);

  String json = "0";

  File file = SPIFFS.open("/export.json");

  if(!file){
      Serial.println("Failed to open file for reading");
      json = "0";
      return json;

  }

  Serial.println("File Content:");


  while(file.available()){
    doc["game"] = file.read();
    Serial.println("Here");
    // Serial.println(doc["game"]);
      
    // Serialize JSON document
    serializeJson(doc, json);
  }
  file.close();

  return json;
}

String simpleJson(){
  return "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
}

char highfive() {
  return '5';
}


String helloWorld() {
  String res = "Hello World";
  return res;
}

void passIt(const String& txt) {
  Serial.println(txt);
}


void setup() {

  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);


  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Try functions:
  //Creating a JSON object from the JSON document


//  JsonObject signal_entry;
//  //Assigning correct values to keys in JSON object
//  signal_entry["date"] = "a";
//  signal_entry["date2"] = "b";
//  signal_entry["date3"] = "c";
//    
//  serializeJsonPretty(signal_entry, Serial); 
//  Serial.println("");

  // JsonArray arrai = array_doc.to<JsonArray>();

  // arrai.add("Hello");
  // arrai.add(42);
  // arrai.add(3.14);
  // serializeJson(array_doc, Serial);
  


  
  // Serial.println(getIt());
  // Serial.println(getJsonFromFile());




  //passIt("Txt");
  //add2csv();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone
  // GMT +1 = 3600
  // GMT +2 = 7200
  // ...
  timeClient.setTimeOffset(7200);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/motor_calibrator.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/motor_calibrator.html", String(), false, processor);
  });

  // Route for position of motor calibration
  server.on("/save_position", HTTP_GET, [](AsyncWebServerRequest * request) {

    // TODO: Display on the screen (e.g.) Calibration successful 
    // ##
    // ##
    // ##
    // ##
    // ##
    
    request->send(SPIFFS, "/save_position.html", String(), false, processor);
  });
  
  // Route for page motor calibration
  server.on("/motor_calibrator", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/motor_calibrator.html", String(), false, processor);
  });
  
  // Route for json export
  server.on("/export", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/export.json", String(), false, processor);
    });
    
  // Route for json export
  server.on("/e2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "simpleJson()", String(), false, processor);
    });


  // Route for highfive
  server.on("/highfive", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  int formattedHours = timeClient.getHours();
  Serial.println(formattedHours);


//  StaticJsonBuffer<300> JSONbuffer;
//  JsonObject& JSONencoder = JSONbuffer.createObject();
//
//  JSONencoder["Hour"] = formattedHours;
//  JsonArray& values = JSONencoder.createNestedArray("values");
//  values.add(20);
//  values.add(21);
//  values.add(23);
//
//  char JSONmessageBuffer[300];
//  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
//  add2json(JSONmessageBuffer);

  
  // Serial.println(JSONmessageBuffer);
  // Serial.println();

  delay(10000);

}
