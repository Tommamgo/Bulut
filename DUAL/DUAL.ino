
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
// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;

// Motor
#define motor1Pin1 33
#define motor1Pin2 26
#define enable1Pin 14

// Button 
#define button 16
boolean turn_start = false;
boolean turn_stop = true;  



// Interrupt function to detect a new Coin
// Need to be over the SetUp function for init
void IRAM_ATTR coinINPUT(){
  //Serial.print("We got an new Coin");
  credit_conut50++;
  //Serial.println("Credit balance: " + String(credit, 2));
}

void IRAM_ATTR button_press(){
  if(turn_start){
    turn_start = false;
  }else{
    turn_start = true; 
  }
  if(turn_stop){
    turn_stop = false;
  }else{
    turn_stop = true;
  }
}
void strat_stop(){
  if(digitalRead(button)){
    turn_start = false; 
    turn_stop = true;
  }else{
    turn_start = true; 
    turn_stop = false;
  }
}

float getfloatCredit(){
  return credit_conut50 * 0.50;
}





void setup() {
  Serial.begin(115200); 
  Serial.println("Hallo ich bin am Leben");

  // Button-------------------------------------------------------------------------------------------------
  pinMode(button, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(button), button_press, LOW); // fot interrupt_Mode
  //--------------------------------------------------------------------------------------------------------

  //Display-------------------------------------------------------------------------------------------------
  display.init();
  display.setFont(ArialMT_Plain_24);
  display.drawString(80, 32, "0.00");
  display.display();
  delay(100);

  //--------------------------------------------------------------------------------------------------------
  // Motor--------------------------------------------------------------------------------------------------
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  //---------------------------------------------------------------------------------------------------------
  

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
                    (void*)&credit_conut50,        /* parameter of the task */
                    1,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 

    //-------------------------------------------------------------------------------------------------------

    // Muenzpruefer--------------------------------------------------------------------------------------------
    attachInterrupt(digitalPinToInterrupt(Muenzpruefer_Pin), coinINPUT, RISING); // fot interrupt_Mode
    

   
}

void stop_motor(){
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(1000);
}



//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    strat_stop();
    if (turn_start && credit_conut50 > 0 ){
      credit_conut50--;
      // Move DC motor forward with increasing speed
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      Serial.println(turn_start);
      Serial.println(turn_stop);
      while (dutyCycle <= 255 && turn_start == true && turn_stop == false){
        ledcWrite(pwmChannel, dutyCycle);   
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        if(dutyCycle < 255){
          dutyCycle = dutyCycle + 5;
        }else{
          strat_stop();
        }
        delay(50);
    }
    delay(1000);
    while(dutyCycle > 0 && turn_start == false && turn_stop == true){
       ledcWrite(pwmChannel, dutyCycle);
       Serial.print("Forward with duty cycle: ");
       Serial.println(dutyCycle);
       dutyCycle-=5;
       delay(75);
    }
     Serial.println("Motor stopped");
     digitalWrite(motor1Pin1, LOW);
     digitalWrite(motor1Pin2, LOW);
    dutyCycle = 0;
    delay(100);
    }
    delay(100);
  } 
}

//Task2code: 
void Task2code(void * coin_count ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  int credit_state = 0;
  for(;;){
    if(credit_state != *((int*)coin_count)){
      display.clear();
      delay(10);
      display.drawString(80, 32, String(getfloatCredit(), 2));
      display.display(); 
      credit_state = *((int*)coin_count); 
    }
   //Serial.println(*((int*)coin_count));
    delay(10);

  }
}

// DO NOT USE!!!!!
void loop() {
  
}
