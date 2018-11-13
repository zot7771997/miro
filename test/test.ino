#include <miro.h>

Miro robot;

void setup() {
  // put your setup code here, to run once:
  robot.init();
  robot.sonarOn(400);
  Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(10, 255);
  robot.ledOn(LEFT, 255);
  delay(10);
  //analogWrite(10, 0);
  robot.ledOff(LEFT);
  delay(10);
  float range = 0;
  digitalWrite(PIN_MOTOR_LB, LOW);
  analogWrite(PIN_MOTOR_LA_PWM, 0);
  //robot.wheelRotateAng(-1, 360, 1, 360, false);
  range = robot.wheelGetAngSpeed(LEFT);
  Serial.print("range ");
  Serial.println(range);
  robot.sync();
}
