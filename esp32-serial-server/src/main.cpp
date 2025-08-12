#include <Arduino.h>
#include "serial_comm.h"
#include "command_functions.h"


void IRAM_ATTR readEncoderA()
{
  if (digitalRead(encA.clkPin) == digitalRead(encA.dirPin))
  {
    encA.tickCount -= 1;
  }
  else
  {
    encA.tickCount += 1;
  }
}

void IRAM_ATTR readEncoderB()
{
  if (digitalRead(encB.clkPin) == digitalRead(encB.dirPin))
  {
    encB.tickCount -= 1;
  }
  else
  {
    encB.tickCount += 1;
  }
}

void encoderInit()
{
  encA.setPulsePerRev(encA_ppr);
  encB.setPulsePerRev(encB_ppr);

  attachInterrupt(digitalPinToInterrupt(encA.clkPin), readEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(encB.clkPin), readEncoderB, RISING);
}

void velFilterInit()
{
  velFilterA.setOrder(1);
  velFilterA.setCutOffFreq(velFilterCutOffFreqA);

  velFilterB.setOrder(1);
  velFilterB.setCutOffFreq(velFilterCutOffFreqB);
}

void pidInit()
{
  pidMotorA.setParameters(kpA, kiA, kdA, outMin, outMax);
  pidMotorB.setParameters(kpB, kiB, kdB, outMin, outMax);
  pidMotorA.begin();
  pidMotorB.begin();
}


//---------------------------------------------------------------------------------------------
// Timing variables
unsigned long sensorUpdateTime, sensorUpdateTimeInterval = 5;
unsigned long serialLoopTime, serialLoopTimeInterval = 5;
unsigned long pidTime, pidTimeInterval = 20;
// unsigned long pidStopTime, pidStopTimeInterval = 250;
//---------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  encoderInit();
  velFilterInit();
  pidInit();

  // Initialize timing markers
  unsigned long now = millis();
  // unsigned long now_us = micros();
  sensorUpdateTime = now;
  serialLoopTime   = now;
  pidTime          = now;
}

void loop()
{
  unsigned long now = millis();
  // unsigned long now_us = micros();

  // Sensor update loop
  if ((now - sensorUpdateTime) >= sensorUpdateTimeInterval)
  {
    unfilteredVelA = encA.getAngVel();
    unfilteredVelB = encB.getAngVel();

    filteredVelA = velFilterA.filter(unfilteredVelA);
    filteredVelB = velFilterB.filter(unfilteredVelB);

    sensorUpdateTime = now;
  }
  
  // Serial comm loop
  if ((now - serialLoopTime) >= serialLoopTimeInterval)
  {
    recieve_and_send_data();
    serialLoopTime = now;
  }

  // PID control loop
  if ((now - pidTime) >= pidTimeInterval)
  {
    if (pidMode)
    {
      outputA = pidMotorA.compute(targetA, filteredVelA);
      outputB = pidMotorB.compute(targetB, filteredVelB);

      motorA.sendPWM((int)outputA);
      motorB.sendPWM((int)outputB);
    }
    pidTime = now;
  }
}