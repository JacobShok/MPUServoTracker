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