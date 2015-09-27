#include <math.h>
#include "UltrasonicSensor.h"
#include "Motor.h"

#define STARTUP_DELAY 100
#define SENSOR_DELAY 10
#define CONVERSION_DIVISOR 58

UltrasonicSensor ultrasonic1;
UltrasonicSensor ultrasonic2;

Motor motorLeft;
Motor motorRight;

long sensorLeftRunning[] = {0, 0, 0};
long sensorRightRunning[] = {0, 0, 0};

double steeringFactor = 1.5;

void updateUltrasonicSensor(UltrasonicSensor *sensor) {
  digitalWrite(sensor->pinOut, HIGH);
  delayMicroseconds(SENSOR_DELAY);
  digitalWrite(sensor->pinOut, LOW);

  sensor->readOut = pulseIn(sensor->pinIn, HIGH);
}

int steeringPos = 90;

int steeringRatio(long cmLeft, long cmRight) {
  long delta = cmRight - cmLeft;
  double deltaPercent = ((double) delta * steeringFactor) / (double) (cmLeft + cmRight);
  return (int) round(deltaPercent * 100);
}

long averageWithoutOutliers(long sensorData[]) {
  long delta12 = abs(sensorData[1] - sensorData[2]);
  long delta23 = abs(sensorData[2] - sensorData[3]);
  long delta13 = abs(sensorData[1] - sensorData[3]); 

  // If delta 1 2 is less than delta 1 3 and less than delta 2 3, 1 and 2 are closest together.

  if(delta12 < delta23 && delta12 < delta13) {
    return round(((double) sensorData[1] + (double) sensorData[2]) / 2.0);
  } else if(delta13 < delta23 && delta13 < delta12) {
    return round(((double) sensorData[1] + (double) sensorData[3]) / 2.0);
  } else if(delta23 < delta13 && delta23 < delta12) {
    return round(((double) sensorData[2] + (double) sensorData[3]) / 2.0);
  } else {
    return sensorData[1];  
  }
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

  sensorLeftRunning[2] = sensorLeftRunning[1];
  sensorLeftRunning[1] = sensorLeftRunning[0];
  sensorLeftRunning[0] = cml;

  sensorRightRunning[2] = sensorRightRunning[1];
  sensorRightRunning[1] = sensorRightRunning[0];
  sensorRightRunning[0] = cmr;

  int sr = steeringRatio(averageWithoutOutliers(sensorLeftRunning), averageWithoutOutliers(sensorRightRunning));

  // Update motors

  motorLeft.powerLevel = 100;
  motorRight.powerLevel = 100;

  if(cml < 40 || cmr < 40) {
    if(sr < 0) {
      motorLeft.powerLevel = 100 + sr;
      if(motorLeft.powerLevel < 0) {
        motorLeft.powerLevel = 0;
      }
    } else {
      motorRight.powerLevel = 100 - sr;
      if(motorRight.powerLevel < 0) {
        motorRight.powerLevel = 0;
      }
    }
  }

  // Print information to console

  if(cml < 40 || cmr < 40) {

    Serial.print("Motor 1: ");
    Serial.print(motorLeft.powerLevel);
    Serial.print(" %");
    Serial.print(" | ");
    Serial.print(cml);
    Serial.println();
  
    Serial.print("Motor 2: ");
    Serial.print(motorRight.powerLevel);
    Serial.print(" %");
    Serial.print(" | ");
    Serial.print(cmr);
    Serial.println();
  
    Serial.print("Ratio: ");
    Serial.print(sr);
    Serial.println("");
    Serial.println();

  }

  if(motorLeft.on) {
    analogWrite(motorLeft.pinOut, (int) round((float) motorLeft.powerLevel / 100.0 * 255.0));
  } else {
    analogWrite(motorLeft.pinOut, 0);
  }

  if(motorRight.on) {
    analogWrite(motorRight.pinOut, (int) round((float) motorRight.powerLevel / 100.0 * 255.0));
  } else {
    analogWrite(motorRight.pinOut, 0);
  }

  delay(60);
}
