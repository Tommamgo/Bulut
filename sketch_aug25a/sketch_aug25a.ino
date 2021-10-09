// Import required libraries
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "NTPClient.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// start the duo Core Task (Class)
TaskHandle_t Task1;
TaskHandle_t Task2;

// Display
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Muenzpruefung
#define Muenzpruefer_Pin 27
// The Cerdit (the Compile need the volatile because of the Interrupt)
volatile int credit_conut50 = 0;

// Motor
#define motor_control 26

// Button
#define button 4
boolean turn_start = false; // not sure if they are needed in future
boolean turn_stop = true;   // ""
boolean on_off = false;

// Julian-Code
const int ledPin = 2;
// Stores LED state
String ledState;
// Julian Code


// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(2)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void add2json(const String& line){
  File file = SPIFFS.open("/export.json", FILE_WRITE);
  if(file.print(line)){
    Serial.println("File was written");
  }else {
    Serial.println("File write failed");
  }
  file.close();
}

char highfive(){
  return '5';
}


String helloWorld(){
  String res = "Hello World";
  return res;
}

void passIt(const String& txt){
  Serial.println(txt);
}










//#######################################################################


// Interrupt function to detect a new Coin
// Need to be over the SetUp function for init
void IRAM_ATTR coinINPUT() {
  //Serial.print("We got an new Coin");
  credit_conut50++;
  //Serial.println("Credit balance: " + String(credit, 2));
}

void IRAM_ATTR switch_on_off() {
  on_off = true;
}

// make the coin count to an float Value
float getfloatCredit() {
  return credit_conut50 * 0.50;
}

// will change the game state, start stop
bool start_stop() {
  if (on_off) {
    return false;
  }
  return true;

}


// Function to scroll text
// The function acepts the following arguments:
// row: row number where the text will be displayed
// message: message to scroll
// delayTime: delay between each character shifting
// lcdColumns: number of columns of your LCD
void scrollText(int row, 
                String message, 
                int delayTime, 
                int lcdColumns, int *playing) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    if(*playing > 0){
      lcd.clear(); 
      break; 
    }
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

bool isWinning(){
  // @TODO in the future this function will check if the Player have won
  // it will compare the the value off the winning possition and the 
  // current value of the possition. This all will be done with an Endcoder
  
  return false; 
}


void setup() {
  Serial.begin(115200);
  Serial.println("Hallo ich bin am Leben");

  // Motor--------------------------------------------------------------------------------------------------
  // sets the pins as outputs:
  pinMode(motor_control, OUTPUT);

  // Muenzpruefer--------------------------------------------------------------------------------------------
  attachInterrupt(digitalPinToInterrupt(Muenzpruefer_Pin), coinINPUT, RISING); // fot interrupt_Mode

  // Button-----------------------------------------------------------------------
  attachInterrupt(digitalPinToInterrupt(button), switch_on_off, RISING);
  
  
  
  //Display-------------------------------------------------------------------------------------------------
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();


  // Pleace do not touch!!!
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,                      /* Task function. */
    "Task2",                        /* name of task. */
    10000,                          /* Stack size of task */
    (void*)&credit_conut50,         /* parameter of the task */
    1,                              /* priority of the task */
    NULL,                           /* Task handle to keep track of created task */
    1);                             /* pin task to core 1 */
  delay(500);




  // Julian-Code????????????????????????????????????????????????????????????????
  pinMode(2, OUTPUT);

    // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Julian Ende
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ) {
  
  // Will show the Task informations in Serial
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    Serial.println(on_off);
    
    // check credit and if the Play wanna start
    //if (on_off && credit_conut50 > 0 ) {
    // Das muss wieder raus #######Wichtig!!!!
    if (on_off) {
      credit_conut50++;
      // bis hier 
      Serial.println("Hallo");
      Serial.println(on_off);
      
      // pay for the round
      // credit_conut50--; ######## WICHTIG MUSS WIEDER REIN!!!!!
      digitalWrite(motor_control, HIGH);
      delay(1000);
      on_off = start_stop();
      while (!on_off) {
        Serial.println("Ich warte gerade auf ein neues Kommando!"); 
        delay(100); 
      }
      start_stop();
      digitalWrite(motor_control, LOW);
    }
    digitalWrite(motor_control, LOW);

    delay(100);
  }

}

//Task2code: blinks an LED every 700 ms
void Task2code( void * coin_count ) {
  // Linus-Code+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  // Proof if the Variable if there is some credit
  Serial.println(credit_conut50);
  float the_money = 0.0; 
  String dd_0 = "Einsatz: "; 
  String messageToScroll = 
  "Gewinne in dem du es schafft im richten Moment zu stoppen und oben beide Taler aufeinanderliegen!";
  // Ende-Linus-----------------------------------------------------------------

  // Julian Code*************************************************************
  // Replace with your network credentials
  const char* ssid = "Schiele";
  const char* password = "1234567898";
  
  // Define an NTP Client to request data and time from an NTP server
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
  
  // Create AsyncWebServer object on port 80
  AsyncWebServer server(80);
  
  // Create a preallocated memory pool to store a Json Object tree
  StaticJsonBuffer<200> jsonBuffer;
  // Set LED GPIO
  const int ledPin = 2;
  // Stores LED state
  String ledState;




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
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
      });
    
      // Route to load style.css file
      server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
      });
    
      // Route to set GPIO to HIGH
      server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
        digitalWrite(ledPin, HIGH);    
        request->send(SPIFFS, "/index.html", String(), false, processor);
      });
      
      // Route to set GPIO to LOW
      server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
        digitalWrite(ledPin, LOW);    
        request->send(SPIFFS, "/index.html", String(), false, processor);
      });
    //  
    //    // Route for json export
    //  server.on("/export.json", HTTP_GET, [](AsyncWebServerRequest *request){
    //    request->send(SPIFFS, getJson(), "text/json");
    //  });
    
        // Route for highfive 
      server.on("/highfive", HTTP_GET, [](AsyncWebServerRequest *request){
        digitalWrite(2, HIGH);    
        request->send(SPIFFS, "index.html", String(), false, processor);
      });
      
      // Start server
      server.begin();
      
  // Julian Funktionen'''''''''''''''''''''''''''''''''''''''''''''''''''''''
  
  // Set LED GPIO
  //int ledPin = 2;
  // Stores LED state
  //String ledState;
  
  // initialize String variables to save the date and time
  String formattedDate;
  String dayStamp;
  String timeStamp;
  
 
  for (;;) {
    
    the_money = *((int*)coin_count);     
    // Will show the Credit when someone is playing
    String dd_0 = "Konto: " + String(the_money, 2) + "E"; 
    Serial.println(*((int*)coin_count)); 
 
    // will show the amound of money when someone plays
    if (*((int*)coin_count) > 0) {
      // multiplies the count of conis with the Value
      the_money = *((int*)coin_count) * 0.5;
      // build the String
      dd_0 = "Konto: " + String(the_money, 2) + "E";
      
      // show the amound off Monry when cash is in 
      lcd.setCursor(0, 0);
      // display it 
      lcd.print(dd_0); 

      delay(100); 

    }else{
      
      // Will display when no Money
      // set cursor to first column, first row
      lcd.setCursor(0, 0);
      lcd.print("TOMATO  50Cent");
     
      // pure horror!!
      // this will give the slide function the possibility to interrupt 
      // when a Player pay the price to play and will show the new 
      // Display in the Gamingmode
      int* point_change = &(*((int*)coin_count)); 
      
      // will display the introduction in slide mode 
      scrollText(1, messageToScroll, 250, lcdColumns, point_change );
      
     
      delay(100); 
    }
    delay(10);

    // Julian-CodePPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
        while(!timeClient.update()) {
        timeClient.forceUpdate();
        }
    
      int formattedHours= timeClient.getHours();
      Serial.println(formattedHours);
    
      StaticJsonBuffer<300> JSONbuffer;
      JsonObject& JSONencoder = JSONbuffer.createObject();
    
      JSONencoder["Hour"] = formattedHours;
      JsonArray& values = JSONencoder.createNestedArray("values");
      values.add(20);
      values.add(21);
      values.add(23);
    
      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      add2json(JSONmessageBuffer);
      // Serial.println(JSONmessageBuffer);
      // Serial.println();

    // Julian-CodePPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
    
  }

}


// DO NOT USE!!!!!
void loop() {

}
