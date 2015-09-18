#include <math.h>
#include <Servo.h>
#include "UltrasonicSensor.h"
#include "Motor.h"

#define STARTUP_DELAY 100
#define SENSOR_DELAY 12
#define CONVERSION_DIVISOR 58

UltrasonicSensor ultrasonic1;
UltrasonicSensor ultrasonic2;

Motor motorLeft;
Motor motorRight;

void updateUltrasonicSensor(UltrasonicSensor *sensor) {
  digitalWrite(sensor->pinOut, HIGH);
  delayMicroseconds(SENSOR_DELAY);
  digitalWrite(sensor->pinOut, LOW);

  sensor->readOut = pulseIn(sensor->pinIn, HIGH);
}

Servo steeringServo;
int steeringPin = 9;
int steeringPos = 90;

int steeringRatio(long cmLeft, long cmRight) {
  long delta = cmRight - cmLeft;
  double deltaPercent = delta / (cmLeft + cmRight);
  return 90 + (int) round(deltaPercent);
}

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

  // Set up motor pins and turn them on

  motorLeft.pinOut = 5;
  motorRight.pinOut = 6;

  digitalWrite(motorLeft.pinOut, HIGH);
  digitalWrite(motorRight.pinOut, HIGH);

  motorLeft.on = true;
  motorRight.on = true;
}

void loop() {
  // Update sensors
  
  updateUltrasonicSensor(&ultrasonic1);
  updateUltrasonicSensor(&ultrasonic2);

  long cml = ultrasonic1.readOut / CONVERSION_DIVISOR;
  long cmr = ultrasonic2.readOut / CONVERSION_DIVISOR;

  // Print information to console

  Serial.print("Sensor 1: ");
  Serial.print(cml);
  Serial.print(" cm");
  Serial.println();

  Serial.print("Sensor 2: ");
  Serial.print(cmr);
  Serial.print(" cm");
  Serial.println();

  // Update motors

  if(motorLeft.on) {
    digitalWrite(motorLeft.pinOut, HIGH);
  } else {
    digitalWrite(motorLeft.pinOut, LOW);
  }

  if(motorLeft.on) {
    digitalWrite(motorRight.pinOut, HIGH);
  } else {
    digitalWrite(motorRight.pinOut, LOW);
  }

  // Update steering servo

  steeringPos = steeringRatio(cml, cmr);
  steeringServo.write(steeringPos);

  delay(60);
}
