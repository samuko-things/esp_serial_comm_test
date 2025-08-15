#include <Arduino.h>
#include "command_functions.h"
#include "serial_comm.h"
#include "i2c_comm.h"

void IRAM_ATTR readEncoder0()
{
  if (digitalRead(encoder[0].clkPin) == digitalRead(encoder[0].dirPin))
  {
    encoder[0].tickCount -= 1;
  }
  else
  {
    encoder[0].tickCount += 1;
  }
}

void IRAM_ATTR readEncoder1()
{
  if (digitalRead(encoder[1].clkPin) == digitalRead(encoder[1].dirPin))
  {
    encoder[1].tickCount -= 1;
  }
  else
  {
    encoder[1].tickCount += 1;
  }
}

void encoderInit()
{
  for (int i=0; i<num_of_motors; i+=1){
    encoder[i].setPulsePerRev(enc_ppr[i]);
  }

  attachInterrupt(digitalPinToInterrupt(encoder[0].clkPin), readEncoder0, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder[1].clkPin), readEncoder1, RISING);
}

void velFilterInit()
{
  for (int i=0; i<num_of_motors; i+=1){
    velFilter[i].setCutOffFreq(cutOffFreq[i]);
  }
}

void pidInit()
{
  for (int i=0; i<num_of_motors; i+=1){
    pidMotor[i].setParameters(kp[i], ki[i], kd[i], outMin, outMax);
    pidMotor[i].begin();
  }
}


//---------------------------------------------------------------------------------------------
// Timing variables
// please do not adjust any of the values as it can affect important operations
unsigned long sensorUpdateTime, sensorUpdateTimeInterval = 5;
unsigned long serialLoopTime, serialLoopTimeInterval = 5;
unsigned long pidTime, pidTimeInterval = 20;
unsigned long pidStopTime[2], pidStopTimeInterval = 250;
//---------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin(i2cAddress);

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
  for (int i=0; i<num_of_motors; i+=1){
    pidStopTime[i] = now;
    cmdVelTimeout[i] = now;
    isMotorCommanded[i] = 0;
  }
}

void loop()
{
  unsigned long now = millis();
  // unsigned long now_us = micros();

  // Sensor update loop
  if ((now - sensorUpdateTime) >= sensorUpdateTimeInterval)
  {
    for (int i=0; i<num_of_motors; i+=1)
    {
      unfilteredVel[i] = encoder[i].getAngVel();
      filteredVel[i] = velFilter[i].filter(unfilteredVel[i]);
    }
    sensorUpdateTime = now;
  }
  
  // Serial comm loop  // i2cSendMsg = "";
  if ((now - serialLoopTime) >= serialLoopTimeInterval)
  {
    recieve_and_send_data();
    serialLoopTime = now;
  }

  // PID control loop
  if ((now - pidTime) >= pidTimeInterval)
  {
    for (int i=0; i<num_of_motors; i+=1)
    {
      if (pidMode[i])
      {
        output[i] = pidMotor[i].compute(target[i], filteredVel[i]);
        motor[i].sendPWM((int)output[i]);
      }
    }
    pidTime = now;
  }

  // check to see if motor has stopped
  for (int i=0; i<num_of_motors; i+=1){
    if (fabs(target[i]) < 0.01 && pidMode[1] == 1)
    {
      if ((millis() - pidStopTime[i]) >= pidStopTimeInterval)
      {
        target[i] = 0.00;
        pidMode[i] = 0;
        motor[i].sendPWM(0);
        pidMotor[i].begin();
        isMotorCommanded[i] = 0;
        pidStopTime[i] = millis();
      }
    }
    else
    {
      pidStopTime[i] = millis();
    }
  }
  
  // command timeout
  int cmdTimeout = (int)cmdVelTimeoutInterval;
  if (cmdVelTimeoutInterval > 0)
  {
    for (int i=0; i<num_of_motors; i+=1){
      if (!isMotorCommanded[i]) {
        cmdVelTimeout[i] = millis();
      }
      if (isMotorCommanded[i] && ((millis() - cmdVelTimeout[i]) >= cmdVelTimeoutInterval))
      {
        target[i] = 0.00;
        pidMode[i] = 0;
        motor[i].sendPWM(0);
        pidMotor[i].begin();
        isMotorCommanded[i] = 0;
      }
    }
  }
  
}