#ifndef ENCODER_SETUP_H
#define ENCODER_SETUP_H
#include <Arduino.h>
#include "driver/gpio.h"
#include "esp_timer.h"  // For esp_timer_get_time()


class QuadEncoder {
public:
  volatile long tickCount;
  volatile float dir;
  int clkPin, dirPin;
  float pulsePerRev;
  int64_t stopPeriod_us = 10000;
  volatile int64_t periodPerTick_us, oldTickTime_us;

  QuadEncoder(int clk_pin, int dir_pin, float ppr);

  void setPulsePerRev(float ppr);
  float getAngPos();
  float getAngVel();
  void resetPeriod();

private:
  float prevTickCount;
  
};





QuadEncoder::QuadEncoder(int clk_pin, int dir_pin, float ppr)
{
  clkPin = clk_pin;
  dirPin = dir_pin;
  pulsePerRev = ppr;

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dirPin, INPUT_PULLUP);

  oldTickTime_us = esp_timer_get_time();
  prevTickCount = tickCount;
  dir = 1.00;
}

void QuadEncoder::setPulsePerRev(float ppr)
{
  pulsePerRev = ppr;
}

float QuadEncoder::getAngPos()
{
  return (2.00 * PI * (float)tickCount) / pulsePerRev;
}

float QuadEncoder::getAngVel()
{
  float ang_vel;

  if (periodPerTick_us == 0) 
    ang_vel = 0;
  else 
    ang_vel = dir * 2.00 * PI * 1e6 / ((float)pulsePerRev * (float)periodPerTick_us);

  return ang_vel;
}

void QuadEncoder::resetPeriod()
{
  int64_t t = esp_timer_get_time();
  ;
  if ((t - oldTickTime_us) >= stopPeriod_us)
  {
    periodPerTick_us = 0;
  }
}


#endif