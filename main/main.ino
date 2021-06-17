volatile unsigned int temp, counter = 0; //This variable will increase or decrease depending on the rotation of encoder

int p4 = 5; 
int p5 = 4;
    
void setup() {
  Serial.begin (9600);
  Serial.print("Hallo");
  pinMode(p4, INPUT_PULLUP); // internal pullup input pin 2 
  
  pinMode(p5, INPUT_PULLUP); // internal pullup input pin 3
//Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(p4, ai0, RISING);
   
  //B rising pulse from encodenren activated ai1(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.
  attachInterrupt(p5, ai1, RISING);
  }
   
  void loop() {
  // Send the value of counter
  if( counter != temp ){
  Serial.println (counter);
  temp = counter;
  }
  }
   
  void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(p5)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }
   
  void ai1() {
  // ai0 is activated if DigitalPin nr 3 is going from LOW to HIGH
  // Check with pin 2 to determine the direction
  if(digitalRead(p4)==LOW) {
  counter--;
  }else{
  counter++;
  }
  }
