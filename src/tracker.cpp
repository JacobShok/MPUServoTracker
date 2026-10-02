#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

// vars for gyro
int16_t gyroX, gyroY, gyroZ;
uint8_t hiX, loX, hiY, loY, hiZ, loZ;
float scaled_gyroX, scaled_gyroY, scaled_gyroZ;
float angleX, angleY, angleZ;
float offsetX, offsetY, offsetZ;

// rolling bufs for stats
float* Ybuf;
float* Zbuf;
int tail, head;

// gyro stats
float* stats;

// servo instance
Servo servo_roll;
Servo servo_yaw;

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

void push(float valY, float valZ) {
  Ybuf[head] = valY;
  Zbuf[head] = valZ;

  head = (head + 1) % 128;

  if (head == tail) {
    tail = (tail + 1) % 128;
  }

  return;
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

  push(scaled_gyroY, scaled_gyroZ);

  float dt = (micros() - lastSample) / 1e6;  
  lastSample = micros();

  angleX += scaled_gyroX * dt;
  angleY += scaled_gyroY * dt;
  angleZ += scaled_gyroZ * dt;
}

void moveServos()
{
  servo_roll.write(constrain(90 + angleY, 0, 180));
  servo_yaw.write(constrain(90 + angleZ, 0, 180));
}

void calc_stats()
{
  float sumY = 0, sumZ = 0;
  int count = 0;
  int i = tail;

  while (i != head) {
    sumY += Ybuf[i];
    sumZ += Zbuf[i];
    i = (i + 1) % 128;
    count++;
  }
  if (count == 0) return;

  stats[0] = sumY / count;
  stats[2] = sumZ / count;

  float varY = 0, varZ = 0;
  i = tail;
  while (i != head) {
    varY += (Ybuf[i] - stats[0]) * (Ybuf[i] - stats[0]);
    varZ += (Zbuf[i] - stats[2]) * (Zbuf[i] - stats[2]);
    i = (i + 1) % 128;
  }
  stats[1] = sqrt(varY / count);
  stats[3] = sqrt(varZ / count);
}

void loop()
{
  moveServos();

  readIMU();
  calculateAngleDisplacment();

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