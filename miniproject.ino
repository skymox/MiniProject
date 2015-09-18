#include <Servo.h>
#include "UltrasonicSensor.h"

#define STARTUP_DELAY 100
#define SENSOR_DELAY 12
#define CONVERSION_DIVISOR 58

UltrasonicSensor ultrasonic1;
UltrasonicSensor ultrasonic2;

void updateUltrasonicSensor(UltrasonicSensor *sensor) {
  digitalWrite(sensor->pinOut, HIGH);
  delayMicroseconds(SENSOR_DELAY);
  digitalWrite(sensor->pinOut, LOW);

  sensor->readOut = pulseIn(sensor->pinIn, HIGH);
}

Servo steeringServo;
int steeringPin = 9;
int steeringPos = 90;

void setup() {
  ultrasonic1.pinOut = 7;
  ultrasonic1.pinIn = 8;

  ultrasonic2.pinOut = 12;
  ultrasonic2.pinIn = 13;

  steeringServo.attach(steeringPin);

  Serial.begin(9600);

  pinMode(ultrasonic1.pinOut, OUTPUT);
  pinMode(ultrasonic1.pinIn, INPUT);
  
  digitalWrite(ultrasonic1.pinOut, LOW);

  pinMode(ultrasonic2.pinOut, OUTPUT);
  pinMode(ultrasonic2.pinIn, INPUT);
  
  digitalWrite(ultrasonic2.pinOut, LOW);

  delay(STARTUP_DELAY);
}

void loop() {
  // Update sensors
  
  updateUltrasonicSensor(&ultrasonic1);
  updateUltrasonicSensor(&ultrasonic2);

  // Print information to console

  long cm = ultrasonic1.readOut / CONVERSION_DIVISOR;

  Serial.print("Sensor 1: ");
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();

  cm = ultrasonic2.readOut / CONVERSION_DIVISOR;

  Serial.print("Sensor 2: ");
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();

  // Update steering servo

  steeringServo.write(steeringPos);

  delay(60);
}
