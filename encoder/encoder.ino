#include <ESP32Encoder.h>

ESP32Encoder encoder;
const int encoderA_pin = 12;
const int encoderB_pin = 27;
const int encoder_CPR = 2000;

float count_to_angle(int32_t cnt)
{
  float angle = (float)cnt * 360.0f / (float)encoder_CPR;
  return angle;
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(encoderA_pin, INPUT_PULLUP);
  pinMode(encoderB_pin, INPUT_PULLUP);

  encoder.setCount(0);
  encoder.attachFullQuad(encoderA_pin, encoderB_pin);
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(10);
  Serial.println(count_to_angle(encoder.getCount()));
  Serial.println(encoder.getCount());
}
