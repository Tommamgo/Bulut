#include "SSD1306.h" 
#include <Wire.h>


// start the duo Core Task (Class) 
TaskHandle_t Task1;
TaskHandle_t Task2;


// Display
SSD1306  display(0x3c, 21, 22);

// Muenzpruefung 
#define Muenzpruefer_Pin 27
// The Cerdit 
volatile int credit_conut50 = 0; 

// Motor
#define motor_control 26

// Button 
#define button 4
boolean turn_start = false;
boolean turn_stop = true; 



// Interrupt function to detect a new Coin
// Need to be over the SetUp function for init
void IRAM_ATTR coinINPUT(){
  //Serial.print("We got an new Coin");
  credit_conut50++;
  //Serial.println("Credit balance: " + String(credit, 2));
}

// make the coin count to an float Value
float getfloatCredit(){
  return credit_conut50 * 0.50;
}

// will change the game state, start stop
void strat_stop(){
  
  if(digitalRead(button)){
    Serial.println("button");
    turn_start = false; 
    turn_stop = true;
  }else{
    turn_start = true; 
    turn_stop = false;
  }
}







void setup() {
  Serial.begin(115200); 
  Serial.println("Hallo ich bin am Leben");

  // Motor--------------------------------------------------------------------------------------------------
  // sets the pins as outputs:
  pinMode(motor_control, OUTPUT); 



  // Muenzpruefer--------------------------------------------------------------------------------------------
  attachInterrupt(digitalPinToInterrupt(Muenzpruefer_Pin), coinINPUT, RISING); // fot interrupt_Mode

  // Button-------------------------------------------------------------------------------------------------
  pinMode(button, INPUT);

  //Display-------------------------------------------------------------------------------------------------
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.drawString(80, 32, "0.00");
  display.display();
  delay(100);



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
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    strat_stop();
    // check credit and if the Play wanna start
    if (turn_start && credit_conut50 > 0 ){
      // pay for the round
      credit_conut50--;

      while(turn_start){
        strat_stop();
        digitalWrite(motor_control, HIGH);
        delay(10); 
      }
      digitalWrite(motor_control, LOW); 
      
    }
    digitalWrite(motor_control, LOW); 

    delay(10); 

    

  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  Serial.println(credit_conut50); 

  int credit_state = 0;
  for(;;){
    if(credit_state != credit_conut50){
      display.clear();
      delay(10);
      display.drawString(80, 32, String(getfloatCredit(), 2));
      display.display(); 
      credit_state = credit_conut50; 
    }
    //Serial.println(credit_conut50); 
   //Serial.println(*((int*)coin_count));
    delay(10);

  }

  
}


// DO NOT USE!!!!!
void loop() {
  
}
