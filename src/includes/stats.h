#include <Arduino.h>
#include <Wire.h>

// rolling bufs for stats
extern float* Ybuf;
extern float* Zbuf;
extern int tail, head;

// gyro stats
extern float* stats;

void push(float valY, float valZ);

void calc_stats();