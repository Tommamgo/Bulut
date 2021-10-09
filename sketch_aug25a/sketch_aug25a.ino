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
// The Cerdit
volatile int credit_conut50 = 0;

// Motor
#define motor_control 26

// Button
#define button 4
boolean turn_start = false;
boolean turn_stop = true;
boolean on_off = false;





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
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
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

  // Button-----------------------------------------------------------------------
  attachInterrupt(digitalPinToInterrupt(button), switch_on_off, RISING);

  // Button-------------------------------------------------------------------------------------------------
  // pinMode(button, INPUT);

  //Display-------------------------------------------------------------------------------------------------
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();



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
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    Serial.println(on_off);
    // check credit and if the Play wanna start
    if (on_off && credit_conut50 > 0 ) {
      Serial.println("Hallo");
      Serial.println(on_off);
      // pay for the round
      credit_conut50--;
      digitalWrite(motor_control, HIGH);
      delay(1000);
      on_off = start_stop();
      while (!on_off) {

      }
      start_stop();

      digitalWrite(motor_control, LOW);

    }
    digitalWrite(motor_control, LOW);

    delay(100);
  }

}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  // Proof if the Variable if there is some credit
  Serial.println(credit_conut50);

  int credit_state = 0;
  String dd_0 = "Einsatz: "; 

  String messageToScroll = 
  "Gewinne in dem du es schafft im richten Moment zu stoppen und oben beide Taler aufeinanderliegen!";
  
   
  credit_conut50 = 10 ; 
  for (;;) {
    // Will show the Credit when someone is playing
    String dd_0 = "Konto: " + String(getfloatCredit(), 2) + "E"; 
  
    if (getfloatCredit() > 0) {
      // show the amound off Monry when cash is in 
      lcd.setCursor(0, 0);
      lcd.print(dd_0); 
      
      // weiß gerade nicht was ich damit wollt könnte aber was mit Pinter zu tun haben
      credit_state = credit_conut50;

    }else{
      // Will display when no Money
      // set cursor to first column, first row
      lcd.setCursor(0, 0);
      lcd.print("TOMATO  50Cent");
      // will display the introduction
      scrollText(1, messageToScroll, 250, lcdColumns);
      delay(500); 

    }
    //Serial.println(credit_conut50);
    //Serial.println(*((int*)coin_count));
    delay(10);
  }

}


// DO NOT USE!!!!!
void loop() {

}
