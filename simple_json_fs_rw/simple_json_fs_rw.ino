#include <ArduinoJson.h>
#include "SPIFFS.h"


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
  
  Serial.println("_listdir_:");
  listDir(SPIFFS, "/", 0);
  Serial.println("_writefile_:");
  writeFile(SPIFFS, "/export.json" , DATE_NAME, MODE_NAME, WIN_NAME, CONST_NAME );
  Serial.println("_readfile_:");
  Serial.println(readFile(SPIFFS, "/export.json"));
  Serial.println("_______________");


  delay(1000);          
}


void loop() {
  Serial.println("Loop!");
  readFile(SPIFFS, "/export.json");
  delay(10000);
}
