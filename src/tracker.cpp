#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "includes/gyro.h"
#include "includes/stats.h"

// servo instance
Servo servo_roll;
Servo servo_yaw;

void setup()
{
  // serial console
  Serial.begin(115200);

  // I2C for gyro setup
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Serial.println(Wire.endTransmission(true));
  delay(100);

  calibrate_mpu();

  // servo setup
  servo_roll.attach(9);
  servo_yaw.attach(10);

  servo_roll.write(90);
  servo_yaw.write(90);

  Ybuf = new float[128]();
  Zbuf = new float[128]();
  tail = 0; head = 1; 

  stats = new float[4];

  // record first sample
  lastSample = micros();
}

void moveServos()
{
  servo_roll.write(constrain(90 + angleY, 0, 180));
  servo_yaw.write(constrain(90 + angleZ, 0, 180));
}

void loop()
{
  moveServos();

  readIMU();
  calculateAngleDisplacment();
  push(scaled_gyroY, scaled_gyroZ);

  if (millis() - lastPrint >= 1000) {
    Serial.print("angleX = "); Serial.println(angleX);
    Serial.print("angleY = "); Serial.println(angleY);
    Serial.print("angleZ = "); Serial.println(angleZ);

    calc_stats();
    Serial.print("Mean Y = "); Serial.println(stats[0]);
    Serial.print("SD Y = "); Serial.println(stats[1]);
    Serial.print("Mean Z = "); Serial.println(stats[2]);
    Serial.print("SD Z = "); Serial.println(stats[3]);
    Serial.println("-------------------------------------------");

    lastPrint = millis();
  }
}