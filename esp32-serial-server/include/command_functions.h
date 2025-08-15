#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>
#include <Wire.h>
#include "l298n_motor_control.h"
#include "encoder_setup.h"
#include "adaptive_low_pass_filter.h"
#include "simple_pid_control.h"

//--------------- global variables -----------------//

// motor 0 H-Bridge Connection
int IN1 = 26, IN2 = 27, enA = 25;
// motor 1 H-Bridge Connection
int IN3 = 14, IN4 = 12, enB = 13;

L298NMotorControl motor[2] = {
  L298NMotorControl(IN1, IN2, enA), // motor 0
  L298NMotorControl(IN3, IN4, enB) // motor 1
};


// store encoder pulsePerRev needed by encoder
// float enc0_ppr = 374.25;
// float enc1_ppr = 374.25;
float enc_ppr[2]={
  1000.0, // motor 0 encoder pulse per revolution parameter
  1000.0 // motor 1 encoder pulse per revolution parameter
};

// motor 0 encoder connection
int enc1_clkPin = 18, enc1_dirPin = 19;
// motor 1 encoder connection
int enc2_clkPin = 16, enc2_dirPin = 17;

QuadEncoder encoder[2] = {
  QuadEncoder(enc1_clkPin, enc1_dirPin, enc_ppr[0]), // motor 0 encoder connection
  QuadEncoder(enc2_clkPin, enc2_dirPin, enc_ppr[1]) // motor 1 encoder connection
};

// adaptive lowpass Filter
const int filterOrder = 1;
float cutOffFreq[2] = {
  1.0, // motor 0 velocity filter cutoff frequency
  1.0 // motor 1 velocity filter cutoff frequency
};

AdaptiveLowPassFilter velFilter[2] = {
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[0]), // motor 0 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[1]) // motor 1 velocity filter
};

float filteredVel[2] = {
  0.0,
  0.0
};

float unfilteredVel[2] = {
  0.0,
  0.0
};

// motor PID parameters
float outMin = -255.0, outMax = 255.0;

float kp[2] = {
  0.0,
  0.0
};

float ki[2] = {
  0.0,
  0.0
};

float kd[2] = {
  0.0,
  0.0
};

float target[2] = {
  0.0,
  0.0
};

float output[2] = {
  0.0,
  0.0
};

SimplePID pidMotor[2] = {
  SimplePID(kp[0], ki[0], kd[0], outMin, outMax),
  SimplePID(kp[1], ki[1], kd[1], outMin, outMax),
};


// check if in PID or PWM mode
int pidMode[2] = {
  0,
  0
}; // 1-PID MODE, 0-SETUP/PWM MODE

int isMotorCommanded[2] = {
  0,
  0
};

float rdir[2] = {
  1.0,
  1.0
};

// // maximum motor velocity that can be commanded
float maxVel[2] = {
  10.0,
  10.0
};

// for command timeout.
unsigned long cmdVelTimeoutInterval = 0; // ms -> (1000/sampleTime) hz
unsigned long cmdVelTimeout[2];

// initial i2cAddress
byte i2cAddress = 0x55;
//-------------------------------------------------//






//--------------- global functions ----------------//

String readPos(int motor_no){
  float posData = encoder[motor_no].getAngPos();
  String data = String(posData, 3);
  return data;
}

String readVel(int motor_no){
  String data = String(filteredVel[motor_no], 4);
  data += ",";
  data += String(unfilteredVel[motor_no], 4);
  return data;
}


String readPidVel(int motor_no)
{
  String data = String(target[motor_no], 4);
  data += ",";
  data += String(filteredVel[motor_no], 4);
  return data;
}


String writePWM(int motor_no, int pwm)
{
  pidMode[motor_no] = 0;
  motor[motor_no].sendPWM(pwm);
  cmdVelTimeout[motor_no] = millis();
  isMotorCommanded[motor_no] = 1;
  return "1";
}


String writeSpeed(int motor_no, float targetVel)
{
  pidMode[motor_no] = 1;
  target[motor_no] = targetVel;
  cmdVelTimeout[motor_no] = millis();
  isMotorCommanded[motor_no] = 1;
  return "1";
}


// String setPidModeFunc(int motor_no, int mode)
// {
//   pidMode[motor_no] = mode;
//   motor[motor_no].sendPWM(0);
//   pidMotor[motor_no].begin();

//   return "1";
// }
// String getPidModeFunc(int motor_no)
// {
//   return String(pidMode[motor_no]);
// }


String setEncoderPPR(int motor_no, float ppr)
{
  enc_ppr[motor_no] = ppr;
  encoder[motor_no].setPulsePerRev(enc_ppr[motor_no]);
  return "1";
}
String getEncoderPPR(int motor_no)
{
  return String(enc_ppr[motor_no]);
}


String setMotorKp(int motor_no, float Kp)
{
  kp[motor_no] = Kp;
  pidMotor[motor_no].setKp(kp[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKp(int motor_no)
{
  return String(kp[motor_no], 4);
}


String setMotorKi(int motor_no, float Ki)
{
  ki[motor_no] = Ki;
  pidMotor[motor_no].setKi(ki[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKi(int motor_no)
{
  return String(ki[motor_no], 4);
}


String setMotorKd(int motor_no, float Kd)
{
  kd[motor_no] = Kd;
  pidMotor[motor_no].setKd(kd[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKd(int motor_no)
{
  return String(kd[motor_no], 4);
}


String setRdir(int motor_no, float dir)
{
  if (dir >= 0.0)
    rdir[motor_no] = 1.00;
  else
    rdir[motor_no] = -1.00;

  return "1";
}
String getRdir(int motor_no)
{
  return String(rdir[motor_no]);
}


String setCutoffFreq(int motor_no, float f0)
{
  cutOffFreq[motor_no] = f0;
  velFilter[motor_no].setCutOffFreq(cutOffFreq[motor_no]);
  return "1";
}
String getCutoffFreq(int motor_no)
{
  return String(cutOffFreq[motor_no]);
}


String setCmdTimeout(int timeout)
{
  int cmdTimeout = timeout;
  if (cmdTimeout < 10)
  {
    cmdVelTimeoutInterval = 0;
  }
  else
  {
    cmdVelTimeoutInterval = cmdTimeout;
  }
  return "1";
}
String getCmdTimeout()
{
  return String(cmdVelTimeoutInterval);
}


String setI2cAddress(int address)
{
  if((address <= 0) || (address > 255)){
    return "0";
  }
  else {
    i2cAddress = address;
    Wire.begin((uint8_t)i2cAddress);
    return "1";
  }
}
String getI2cAddress()
{
  return String(i2cAddress);
}


#endif