#include <Arduino.h>
#include <Wire.h>

// vars for gyro
extern int16_t gyroX, gyroY, gyroZ;
extern uint8_t hiX, loX, hiY, loY, hiZ, loZ;
extern float scaled_gyroX, scaled_gyroY, scaled_gyroZ;
extern float angleX, angleY, angleZ;
extern float offsetX, offsetY, offsetZ;

// timing vars
extern unsigned long lastSample, lastPrint;

void readBytes();

void readIMU();

void calculateAngleDisplacment();

void calibrate_mpu();