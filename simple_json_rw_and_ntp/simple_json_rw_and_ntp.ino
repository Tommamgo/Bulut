#include <WiFi.h>
#include "time.h"

#include <ArduinoJson.h>
#include "SPIFFS.h"

#define JSONBUFFERLENGTH 200
#define SAVEPOINT "/export.json"

const char* ssid     = "Schiele";
const char* password = "1234567898";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


// Create new file objects (read/write)
File r_file = SPIFFS.open("/export.json");
File w_file = SPIFFS.open("/export.json", FILE_APPEND);

// This is the Buffer length of the JSON write
#define JSONBUFFERLENGTH 200

// Json key values
const String DATE_NAME  = "date";
const String MODE_NAME  = "mode";
const String WIN_NAME   = "win_int";
const String CONST_NAME = "consts";


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

bool merge(JsonObject dest, JsonObjectConst src) {
    for (auto kvp : src) {
        if (kvp.value().is<JsonObject>()) {
            if (!dest.containsKey(kvp.key())) {
                if (!dest.createNestedObject(kvp.key()))
                    return false;
            }
            if (!merge(dest[kvp.key()], kvp.value()))
                return false;
        }
        else {
            if (!dest[kvp.key()].set(kvp.value()))
                return false;
        }
    }
    return true;
}


/////////////////////////////////////////////////////////////////////////////
//void write_to_fs(String to_write){
//  if (w_file.print(to_write)) {
//    Serial.println("File was written");
//  } else {
//    Serial.println("File write failed");
//  }
//  w_file.close();
//}
//
//void read_from_fs(){
//  // Ensure that file exists
//  if (!r_file) {
//    Serial.println("Failed to open file for reading");
//    return;
//  }
//
//  while (r_file.available()) {  
//    Serial.println("File Content:");
//    Serial.write(r_file.read());
//  }
//  
//  // need to close after use
//  r_file.close();
//}
//
//// Write to the SPIFFS
//void write_json(String date, String gamemode, String win, String costs){
//  DynamicJsonDocument  doc(JSONBUFFERLENGTH);
//
//  doc[DATE_NAME]    = date;
//  doc[MODE_NAME]    = gamemode;
//  doc[WIN_NAME]     = win;
//  doc[CONST_NAME]   = costs;
//
//  serializeJson(doc, w_file);
//  w_file.close(); 
//}
/////////////////////////////////////////////////////////////////////////////


void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print(" DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print(" FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void writeFile(
  fs::FS &fs, const char * path,
  String date, String gameMode, String win, String constName ) {

  Serial.printf("Writing file: %s\r\n", path);
  // here the hole Json will be store
  DynamicJsonDocument doc(JSONBUFFERLENGTH); // Buffer
  // Date will passed
  doc[DATE_NAME] = date;
  doc[MODE_NAME] = gameMode;
  doc[WIN_NAME] = win;
  doc[CONST_NAME] = constName;

  // made the json
  String json_String;
  serializeJsonPretty(doc, json_String);

  //
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    // If there is no File to Append then ist will create on
    Serial.println("- failed to open file for writing");

  }
  if (file.print(json_String)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

// readed from the SPIFFS
String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return "";
  }

  String readed_stuff;

  Serial.println("- read from file:");
  while (file.available()) {
    //Serial.write(file.read());
    readed_stuff.concat( (char)file.read() );
  }
  file.close();

  // readed_stuff = readed_stuff + "]}";
  return readed_stuff;
}

String readvalidjson(){
  String readed_stuff = readFile(SPIFFS, SAVEPOINT);
  readed_stuff = readed_stuff + "]}";
  return readed_stuff;
}


String create_json(String g_mode, String g_win_it){
  DynamicJsonDocument doc(JSONBUFFERLENGTH); // Buffer

  // doc["game"]["mode"] = "is_real";
  // doc["game"]["win_it"] = "no";

  doc["game"]["mode"] = g_mode;
  doc["game"]["win_it"] = g_win_it;
  doc["game"]["costs"] = "50 Cent";

  
  String json_1;
  serializeJsonPretty(doc, json_1);
  delay(1000);
  return json_1;
}

void add_json(String g_mode, String g_win_it){
  DynamicJsonDocument doc1(JSONBUFFERLENGTH); // Buffer

  doc1["time"] = localTime();
  doc1["mode"] = g_mode;
  doc1["win_it"] = g_win_it;
  
  String json_1;
  serializeJsonPretty(doc1, json_1);


  // Read from File
  DynamicJsonDocument doc2(JSONBUFFERLENGTH); // Buffer
  String json_2 = readFile(SPIFFS, SAVEPOINT);
  // serializeJsonPretty(doc2, json_2);

  // JsonObject obj = doc.as<JsonObject>();

  // merge(doc1.as<JsonVariant>(), doc2.as<JsonVariant>());
  
  // {   "game":[ ..
  String json_combine = json_2 + "," + json_1;
  // .. ]}
  
  // Serial.println("________json-combine_______");
  // Serial.println(json_combine);
  // Serial.println("___________________________");
  
  File file = SPIFFS.open(SAVEPOINT, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
  }
  if (file.print(json_combine)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}



void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  
  while (!Serial) continue;

  // this is needed to comunicate with the fileSystem
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  delay(5000);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int counter = 5;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter--){
      delay(500);
      Serial.print(".");
    }else{
      break;
    }

  }
  Serial.println("");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  localTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

//  Serial.println("_listdir_:");
//  listDir(SPIFFS, "/", 0);
//  Serial.println("_writefile_:");
//  writeFile(SPIFFS, SAVEPOINT , DATE_NAME, MODE_NAME, WIN_NAME, CONST_NAME );
//  Serial.println("_readfile_:");
//  Serial.println(readFile(SPIFFS, SAVEPOINT));
//  Serial.println("_______________");
//  delay(1000);          
}


void loop() {
  Serial.println("Loop!");
  // Serial.println(readFile(SPIFFS, SAVEPOINT));
  Serial.println(readvalidjson());
  delay(5);
  add_json(String(random(0,10)), String(random(0,10)));
  delay(20000);
}
