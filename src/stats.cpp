#include "includes/stats.h"

// rolling bufs for stats
float* Ybuf;
float* Zbuf;
int tail, head;

// gyro stats
float* stats;

void push(float valY, float valZ) {
    Ybuf[head] = valY;
    Zbuf[head] = valZ;
  
    head = (head + 1) % 128;
  
    if (head == tail) {
      tail = (tail + 1) % 128;
    }
  
    return;
  }

  void calc_stats()
{
  float sumY = 0, sumZ = 0;
  int count = 0;
  int i = tail;

  // calc sums
  while (i != head) {
    sumY += Ybuf[i];
    sumZ += Zbuf[i];
    i = (i + 1) % 128;
    count++;
  }
  if (count == 0) return;

  stats[0] = sumY / count; // mean Y
  stats[2] = sumZ / count; // mean Z

  //compute variance for Y and Z
  // Variance = average of squared differences from the mean
  float varY = 0, varZ = 0;
  i = tail;
  while (i != head) {
    // Subtract the mean, then square it — measures how far tha sample is from average
    varY += (Ybuf[i] - stats[0]) * (Ybuf[i] - stats[0]);
    varZ += (Zbuf[i] - stats[2]) * (Zbuf[i] - stats[2]);
    i = (i + 1) % 128;
  }

  // Divide summed squares by count to get variance, then sqrt gives sd
  // higher the sd more noise
  stats[1] = sqrt(varY / count); // stddev Y
  stats[3] = sqrt(varZ / count); // stddev Z
}