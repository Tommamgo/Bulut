/*Example sketch to control a stepper motor with A4988 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */
// Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 3
const uint8_t button = 5;
#define stepsPerRevolution 2000
void setup() {
  Serial.begin(9600);
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(button, INPUT);
}
void loop() {
  if (button == HIGH){
    Serial.println("Hallo ich bin gedrpck ");
  }
  stepper_turn();
  delay(1000);
}

void stepper_turn(){
    //Spin the stepper motor 5 revolutions fast:
  for (int i = 0; i < 5 * stepsPerRevolution; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500); // 500
  }
}
