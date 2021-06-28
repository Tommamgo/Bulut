
#include <Wire.h>
#include "SSD1306.h" 


// Display
SSD1306  display(0x3c, 21, 22);

// Prozesse
TaskHandle_t Task1;
TaskHandle_t Task2;

// Muenzpruefer
#define Muenzpruefer_Pin 27
volatile int credit_conut50 = 0; 


// Interrupt function to detect a new Coin
// Need to be over the SetUp function for init
void IRAM_ATTR coinINPUT(){
  //Serial.print("We got an new Coin");
  credit_conut50++;
  //Serial.println("Credit balance: " + String(credit, 2));
}

float getfloatCredit(){
  return credit_conut50 * 0.50;
}



void setup() {
  Serial.begin(115200); 
  Serial.println("Hallo ich bin am Leben");

  //Display-------------------------------------------------------------------------------------------------
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.drawString(80, 32, "0.50");
  display.display();

  //--------------------------------------------------------------------------------------------------------
  

  // Multiprozessing-----------------------------------------------------------------------------------------
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
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 

    //-------------------------------------------------------------------------------------------------------

    // Muenzpruefer--------------------------------------------------------------------------------------------
    attachInterrupt(digitalPinToInterrupt(Muenzpruefer_Pin), coinINPUT, RISING); // fot interrupt_Mode
    

   
}



//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    Serial.println(String(credit_conut50));
  delay(1000);
  } 
}

//Task2code: 
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  int credit_state = 0;
  for(;;){
//    if(credit_state =! credit_conut50){
//      display.drawString(80, 32, String(getfloatCredit(), 2));
//      display.display(); 
//      credit_state = credit_conut50; 
//    }
    delay(1000);

  }
}

// DO NOT USE!!!!!
void loop() {
  
}
