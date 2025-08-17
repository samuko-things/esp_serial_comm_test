#ifndef ENCODER_SETUP_H
#define ENCODER_SETUP_H
#include <Arduino.h>
#include "driver/periph_ctrl.h"

// For critical sections on ESP32
static portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;


class QuadEncoder {
public:
  volatile long tickCount;
  volatile int8_t dir;
  int clkPin, dirPin;
  float pulsePerRev;
  unsigned long stopPeriod_us = 50000;
  volatile unsigned long periodPerTick_us, oldTickTime_us;

  // ---- Moving average buffer ----
  static const int filterWindow = 5;   // number of samples to average
  float velBuffer[filterWindow];
  int velIndex;
  int velCount;
  float velSum;

  QuadEncoder(int clk_pin, int dir_pin, float ppr);

  void setPulsePerRev(float ppr);
  float getAngPos();
  float getAngVel();
  float getFilteredAngVel();
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

  oldTickTime_us = micros();
  prevTickCount = tickCount;
  dir = 1.00;
}

void QuadEncoder::setPulsePerRev(float ppr)
{
  pulsePerRev = ppr;
}

float QuadEncoder::getAngPos()
{
  portENTER_CRITICAL(&encoderMux);
  unsigned long ticks = tickCount;
  portEXIT_CRITICAL(&encoderMux);
  return (2.00 * PI * (float)ticks) / pulsePerRev;
}

float QuadEncoder::getAngVel()
{
  float ang_vel;

  portENTER_CRITICAL(&encoderMux);
  float direction = (float)dir;
  unsigned long periodPerTick = periodPerTick_us;
  portEXIT_CRITICAL(&encoderMux);

  if (periodPerTick == 0) 
    ang_vel = 0;
  else 
    ang_vel = direction * 2.00 * PI * 1e6 / ((float)pulsePerRev * (float)periodPerTick);

  return ang_vel;
}

float QuadEncoder::getFilteredAngVel() {
  float raw = getAngVel();

  // update SMA buffer
  velSum -= velBuffer[velIndex];
  velBuffer[velIndex] = raw;
  velSum += raw;

  velIndex = (velIndex + 1) % filterWindow;
  if (velCount < filterWindow) velCount++;

  return velSum / velCount;
}

void QuadEncoder::resetPeriod()
{
  unsigned long t = micros();
  
  portENTER_CRITICAL(&encoderMux);
  if ((t - oldTickTime_us) >= stopPeriod_us)
  {
    periodPerTick_us = 0;
  }
  portEXIT_CRITICAL(&encoderMux);
}


#endif

























// #ifndef ENCODER_SETUP_H
// #define ENCODER_SETUP_H

// #include <Arduino.h>
// #include "driver/periph_ctrl.h"

// // For critical sections on ESP32
// static portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

// class QuadEncoder {
// public:
//   volatile long tickCount;
//   volatile int8_t dirInt;   // -1 / +1 direction
//   int clkPin, dirPin;
//   float pulsePerRev;
//   unsigned long stopPeriod_us;
//   volatile unsigned long periodPerTick_us, oldTickTime_us;

//   // ---- Moving average buffer ----
//   static const int filterWindow = 5;   // number of samples to average
//   float velBuffer[filterWindow];
//   int velIndex;
//   int velCount;
//   float velSum;

//   QuadEncoder(int clk_pin, int dir_pin, float ppr, unsigned long stop_us = 20000);

//   void setPulsePerRev(float ppr);
//   float getAngPos();
//   float getAngVel();         // raw velocity (rad/s)
//   float getFilteredAngVel(); // filtered velocity (rad/s)
//   void resetPeriod();

// private:
//   float prevTickCount;
// };

// QuadEncoder::QuadEncoder(int clk_pin, int dir_pin, float ppr, unsigned long stop_us)
// {
//   clkPin = clk_pin;
//   dirPin = dir_pin;
//   pulsePerRev = ppr;
//   stopPeriod_us = stop_us;

//   tickCount = 0;
//   dirInt = 1;
//   periodPerTick_us = 0;
//   oldTickTime_us = micros();

//   // SMA init
//   velIndex = 0;
//   velCount = 0;
//   velSum = 0.0f;
//   for (int i=0; i<filterWindow; i++) velBuffer[i] = 0.0f;

//   pinMode(clkPin, INPUT_PULLUP);
//   pinMode(dirPin, INPUT_PULLUP);
// }

// void QuadEncoder::setPulsePerRev(float ppr) {
//   pulsePerRev = ppr;
// }

// float QuadEncoder::getAngPos() {
//   portENTER_CRITICAL(&encoderMux);
//   long ticks = tickCount;
//   portEXIT_CRITICAL(&encoderMux);

//   return (2.0f * PI * (float)ticks) / pulsePerRev;
// }

// float QuadEncoder::getAngVel() {
//   unsigned long period;
//   int8_t dir;
//   portENTER_CRITICAL(&encoderMux);
//   period = periodPerTick_us;
//   dir = dirInt;
//   portEXIT_CRITICAL(&encoderMux);

//   if (period == 0) return 0.0f;
//   return dir * (2.0f * PI * 1e6f) / (pulsePerRev * (float)period);
// }

// float QuadEncoder::getFilteredAngVel() {
//   float raw = getAngVel();

//   // update SMA buffer
//   velSum -= velBuffer[velIndex];
//   velBuffer[velIndex] = raw;
//   velSum += raw;

//   velIndex = (velIndex + 1) % filterWindow;
//   if (velCount < filterWindow) velCount++;

//   return velSum / velCount;
// }

// void QuadEncoder::resetPeriod() {
//   unsigned long t = micros();
//   if ((t - oldTickTime_us) >= stopPeriod_us) {
//     portENTER_CRITICAL(&encoderMux);
//     periodPerTick_us = 0;
//     portEXIT_CRITICAL(&encoderMux);
//   }
// }

// #endif