#include <ArduinoJson.h>
#include "SPIFFS.h"

#define JSONBUFFERLENGTH 200
#define SAVEPOINT "/export.json"

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

  return readed_stuff;
}


String create_json(String g_mode, String g_win_it){
  DynamicJsonDocument doc(JSONBUFFERLENGTH); // Buffer

  // doc["game"]["mode"] = "is_real";
  // doc["game"]["win_it"] = "no";

  doc["game"]["mode"] = g_mode;
  doc["game"]["win_it"] = g_win_it;
  
  String json_1;
  serializeJsonPretty(doc, json_1);
  delay(1000);
  return json_1;
}

void add_json(String g_mode, String g_win_it){
  DynamicJsonDocument doc1(JSONBUFFERLENGTH); // Buffer

  doc1["game"]["mode"] = g_mode;
  doc1["game"]["win_it"] = g_win_it;
  
  String json_1;
  serializeJsonPretty(doc1, json_1);
  Serial.println("___json1____");
  Serial.println(json_1);
  Serial.println("____________");
  delay(1000);

  DynamicJsonDocument doc2(JSONBUFFERLENGTH); // Buffer
  String json_2 = readFile(SPIFFS, SAVEPOINT);
  serializeJsonPretty(doc2, json_2);
  
  Serial.println("___json2____");
  Serial.println(json_2);
  Serial.println("____________");
  
  // JsonObject obj = doc.as<JsonObject>();

  // merge(doc1.as<JsonVariant>(), doc2.as<JsonVariant>());
  

  String json_combine = json_1 + "," + json_2;
  Serial.println("___txt_combine___");
  Serial.println(json_combine);
  Serial.println("____________");
  
  
  serializeJsonPretty(doc1, json_combine);



  DynamicJsonDocument doc_combine(JSONBUFFERLENGTH); // Buffer
  merge(doc_combine.as<JsonVariant>(), doc1.as<JsonVariant>());
  merge(doc_combine.as<JsonVariant>(), doc2.as<JsonVariant>());
  String json_res;
  serializeJsonPretty(doc1, json_res);
  Serial.println("___doc_combine____");
  Serial.println(json_res);
  Serial.println("__________________");






  Serial.println("_____________combine____________");
  Serial.println(json_combine);
  Serial.println("________________________________");

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

  add_json("is_real", "no");
  
  delay(5000); 
//
//  
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
  Serial.println(readFile(SPIFFS, SAVEPOINT));
  add_json(String(random(0,10)), String(random(0,10)));
  delay(10000);
}
