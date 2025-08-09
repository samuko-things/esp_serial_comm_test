#ifndef ENCODER_SETUP_H
#define ENCODER_SETUP_H
#include <Arduino.h>
// #include <util/atomic.h>


class QuadEncoder {
public:
  volatile long tickCount;
  int clkPin, dirPin;
  float pulsePerRev;

  QuadEncoder(int clk_pin, int dir_pin, float ppr);

  void setPulsePerRev(float ppr);
  float getAngPos();
  float getAngVel();

private:
  float prevTickCount;
  unsigned long lastTime = 0;
  
};





QuadEncoder::QuadEncoder(int clk_pin, int dir_pin, float ppr)
{
  clkPin = clk_pin;
  dirPin = dir_pin;
  pulsePerRev = ppr;

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dirPin, INPUT_PULLUP);

  lastTime = micros();
  prevTickCount = tickCount;
}

void QuadEncoder::setPulsePerRev(float ppr)
{
  pulsePerRev = ppr;
}

float QuadEncoder::getAngPos()
{
  // ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  // {
  //   return (2.00 * PI * (float)tickCount) / pulsePerRev;
  // }
  
  return (2.00 * PI * (float)tickCount) / pulsePerRev;
}

float QuadEncoder::getAngVel()
{
  // ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  // {
  //   float freq = 1.0e6 / (float)(micros() - lastTime);
  //   long dTick = tickCount - prevTickCount;
  //   float ang_vel = (2.00 * PI * (float)dTick) / pulsePerRev * freq;
  //   prevTickCount = tickCount;
  //   lastTime = micros();

  //   return ang_vel;
  // }

  float freq = 1.0e6 / (float)(micros() - lastTime);
  long dTick = tickCount - prevTickCount;
  float ang_vel = (2.00 * PI * (float)dTick) / pulsePerRev * freq;
  prevTickCount = tickCount;
  lastTime = micros();

  return ang_vel;
}


#endif