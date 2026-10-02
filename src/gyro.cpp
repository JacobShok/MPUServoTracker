#include "includes/gyro.h"

// vars for gyro
int16_t gyroX, gyroY, gyroZ;
uint8_t hiX, loX, hiY, loY, hiZ, loZ;
float scaled_gyroX, scaled_gyroY, scaled_gyroZ;
float angleX, angleY, angleZ;
float offsetX, offsetY, offsetZ;

// timing vars
unsigned long lastSample = 0, lastPrint = 0;

void readBytes()
{
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);
  hiX = Wire.read();
  loX = Wire.read();
  hiY = Wire.read();
  loY = Wire.read();
  hiZ = Wire.read();
  loZ = Wire.read();
}

void readIMU()
{
  readBytes();
  gyroX = (hiX << 8) | loX;
  gyroY = (hiY << 8) | loY; 
  gyroZ = (hiZ << 8) | loZ; 
}

void calculateAngleDisplacment()
{
  scaled_gyroX = (float)gyroX / 131.0; scaled_gyroX -= offsetX;
  scaled_gyroY = (float)gyroY / 131.0; scaled_gyroY -= offsetY;
  scaled_gyroZ = (float)gyroZ / 131.0; scaled_gyroZ -= offsetZ;

  float dt = (micros() - lastSample) / 1e6;  
  lastSample = micros();

  angleX += scaled_gyroX * dt;
  angleY += scaled_gyroY * dt;
  angleZ += scaled_gyroZ * dt;
}

void calibrate_mpu()
{
  // calibrate mpu
  for (int i = 0; i < 1000; i++) {
    readBytes();
    offsetX += (hiX << 8) | loX;
    offsetY += (hiY << 8) | loY; 
    offsetZ += (hiZ << 8) | loZ; 
  }
  offsetX /= 131000.0;
  offsetY /= 131000.0;
  offsetZ /= 131000.0;
}