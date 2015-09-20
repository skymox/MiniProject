#include <math.h>
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

int steeringPos = 90;

int steeringRatio(long cmLeft, long cmRight) {
  long delta = cmRight - cmLeft;
  double deltaPercent = (double) delta / (double) (cmLeft + cmRight);
  return (int) round(deltaPercent * 100);
}

void setup() {
  ultrasonic1.pinOut = 7;
  ultrasonic1.pinIn = 8;

  ultrasonic2.pinOut = 12;
  ultrasonic2.pinIn = 13;

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

  analogWrite(motorLeft.pinOut, (int) round((float) motorLeft.powerLevel / 100.0 * 255.0));
  analogWrite(motorRight.pinOut, (int) round((float) motorRight.powerLevel / 100.0 * 255.0));

  motorLeft.on = true;
  motorRight.on = true;
}

void loop() {
  // Update sensors
  
  updateUltrasonicSensor(&ultrasonic1);
  updateUltrasonicSensor(&ultrasonic2);

  // Update variables

  long cml = ultrasonic1.readOut / CONVERSION_DIVISOR;
  long cmr = ultrasonic2.readOut / CONVERSION_DIVISOR;

  int sr = steeringRatio(cml, cmr);

  // Print information to console

  Serial.print("Sensor 1: ");
  Serial.print(cml);
  Serial.print(" cm");
  Serial.println();

  Serial.print("Sensor 2: ");
  Serial.print(cmr);
  Serial.print(" cm");
  Serial.println();

  Serial.print("Ratio: ");
  Serial.print(sr);

  // Update motors

  motorLeft.powerLevel = 100;
  motorRight.powerLevel = 100;

  if(steeringRatio < 0) {
    motorLeft.powerLevel = 100 + steeringRatio;
    if(motorLeft.powerLevel < 0) {
      motorLeft.powerLevel = 0;
    }
  } else {
    motorRight.powerLevel = 100 - steeringRatio;
    if(motorRight.powerLevel < 0) {
      motorRight.powerLevel = 0;
    }
  }

  if(motorLeft.on) {
    analogWrite(motorLeft.pinOut, (int) round((float) motorLeft.powerLevel / 100.0 * 255.0));
    analogWrite(motorRight.pinOut, (int) round((float) motorRight.powerLevel / 100.0 * 255.0));
  } else {
    analogWrite(motorLeft.pinOut, 0);
  }

  if(motorLeft.on) {
    analogWrite(motorLeft.pinOut, (int) round((float) motorLeft.powerLevel / 100.0 * 255.0));
    analogWrite(motorRight.pinOut, (int) round((float) motorRight.powerLevel / 100.0 * 255.0));
  } else {
    analogWrite(motorLeft.pinOut, 0);
  }

  delay(60);
}
