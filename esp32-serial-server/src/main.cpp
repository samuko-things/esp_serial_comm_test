#include <Arduino.h>
#include "command_functions.h"
#include "serial_comm.h"
#include "i2c_comm.h"

void IRAM_ATTR readEncoder0()
{
  unsigned long currentTime_us = micros();

  int clkState = digitalRead(encoder[0].clkPin);
  int dirState = digitalRead(encoder[0].dirPin);

  int8_t dir = (clkState == dirState) ? -1 : 1;
  encoder[0].dir = dir;
  encoder[0].tickCount += dir;

  unsigned long period = currentTime_us - encoder[0].oldTickTime_us;
  if (period > 50 && period < 20000000) { // Ignore if > 20 sec or negative
      encoder[0].periodPerTick_us = period;
  }
  encoder[0].oldTickTime_us = currentTime_us;
}

void IRAM_ATTR readEncoder1()
{
  unsigned long currentTime_us = micros();

  int clkState = digitalRead(encoder[1].clkPin);
  int dirState = digitalRead(encoder[1].dirPin);

  int8_t dir = (clkState == dirState) ? -1 : 1;
  encoder[1].dir = dir;
  encoder[1].tickCount += dir;

  unsigned long period = currentTime_us - encoder[1].oldTickTime_us;
  if (period > 50 && period < 20000000) { // Ignore if > 20 sec or negative
      encoder[1].periodPerTick_us = period;
  }
  encoder[1].oldTickTime_us = currentTime_us;
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
unsigned long serialLoopTime, serialLoopTimeInterval = 100;
unsigned long pidTime, pidTimeInterval = 10;
unsigned long pidStopTime[2], pidStopTimeInterval = 250;
//---------------------------------------------------------------------------------------------

float lowTargetVel = 0.00;  // rad/sec
float highTargetVel = 3.142; // rad/sec
bool sendHigh = true;

long ctrlPrevTime;
long ctrlSampleTime = 5000; // millisec


void setup()
{
  loadStoredParams();

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

  delay(1000);
  sendHigh = true;

  // Initialize timing markers
  unsigned long now = millis();
  sensorUpdateTime = now;
  serialLoopTime   = now;
  pidTime          = now;
  for (int i=0; i<num_of_motors; i+=1){
    pidStopTime[i] = now;
    cmdVelTimeout[i] = now;
    isMotorCommanded[i] = 0;
  }
  ctrlPrevTime = now;
}

void loop()
{
  unsigned long now = millis();

  //---------------------------------------------
  if ((now - ctrlPrevTime) >= ctrlSampleTime)
  {
    if (sendHigh)
    {
      for (int i=0; i<num_of_motors; i+=1)
      {
        pidMode[i] = 1;
        target[i] = highTargetVel;
        isMotorCommanded[i] = 1;
      }
      sendHigh = false;
    }
    else
    {
      for (int i=0; i<num_of_motors; i+=1)
      {
        pidMode[i] = 1;
        target[i] = lowTargetVel;
        isMotorCommanded[i] = 1;
      }
      sendHigh = true;
    }
    ctrlPrevTime = now;
  }
  //---------------------------------------------

  // Serial comm loop
  if ((now - serialLoopTime) >= serialLoopTimeInterval)
  {
    // recieve_and_send_data();
    Serial.println((int)output[0]);
    Serial.println((int)output[1]);
    Serial.println(filteredVel[0]);
    Serial.println(filteredVel[1]);
    Serial.println();
    serialLoopTime = now;
  }

  // Sensor update loop
  if ((now - sensorUpdateTime) >= sensorUpdateTimeInterval)
  {
    for (int i=0; i<num_of_motors; i+=1)
    {
      encoder[i].resetPeriod();
      unfilteredVel[i] = encoder[i].getAngVel();
      // filteredVel[i] = encoder[i].getFilteredAngVel();
      filteredVel[i] = velFilter[i].filter(unfilteredVel[i]);
    }
    sensorUpdateTime = now;
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
        output[i] = 0.00;
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
        output[i] = 0.00;
        pidMode[i] = 0;
        motor[i].sendPWM(0);
        pidMotor[i].begin();
        isMotorCommanded[i] = 0;
      }
    }
  }
  
}