#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>
#include "l298n_motor_control.h"
#include "encoder_setup.h"
#include "adaptive_low_pass_filter.h"
#include "simple_pid_control.h"




//--------------- global variables -----------------//

// motor 1 H-Bridge Connection
int IN1 = 26, IN2 = 27, enA = 25;
// motor 2 H-Bridge Connection
int IN3 = 14, IN4 = 12, enB = 13;

L298NMotorControl motor[2] = {
  L298NMotorControl(IN1, IN2, enA), // motor 1
  L298NMotorControl(IN3, IN4, enB) // motor 2
};


// store encoder pulsePerRev needed by encoder
// float enc1_ppr = 374.25;
// float enc2_ppr = 374.25;
float enc_ppr[2]={
  1000.0, // motor 1 encoder pulse per revolution parameter
  1000.0 // motor 2 encoder pulse per revolution parameter
};

// motor 1 encoder connection
int enc1_clkPin = 18, enc1_dirPin = 19;
// motor 2 encoder connection
int enc2_clkPin = 16, enc2_dirPin = 17;

QuadEncoder encoder[2] = {
  QuadEncoder(enc1_clkPin, enc1_dirPin, enc_ppr[0]), // motor 1 encoder connection
  QuadEncoder(enc2_clkPin, enc2_dirPin, enc_ppr[1]) // motor 2 encoder connection
};

// adaptive lowpass Filter
const int filterOrder = 1;
float cutOffFreq[2] = {
  1.0, // motor 1 velocity filter cutoff frequency
  1.0 // motor 2 velocity filter cutoff frequency
};

AdaptiveLowPassFilter velFilter[2] = {
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[0]), // motor 1 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[1]) // motor 2 velocity filter
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

float rdir[2] = {
  1.0,
  1.0
};

// // calcute allowable maximum angular velocity (for overall smooth operation of the whole system)
// float freq_per_tick_allowable = 2000.0; // Hz
// float wA_allowable = 10.00;
// float wB_allowable = 10.00;

// float calc_wA_allowable()
// {
//   wA_allowable = (2 * PI * freq_per_tick_allowable) / encA_ppr;
//   return wA_allowable;
// }

// float calc_wB_allowable()
// {
//   wB_allowable = (2 * PI * freq_per_tick_allowable) / encB_ppr;
//   return wB_allowable;
// }

// // maximum motor velocity that can be commanded
// float maxVelA = calc_wA_allowable(); // in radians/sec
// float maxVelB = calc_wB_allowable(); // in radians/sec

// for command timeout.
unsigned long cmdVelTimeoutInterval = 5000; // ms -> (1000/sampleTime) hz
unsigned long cmdVelTimeout[2] = {
  0,
  0
};
//-------------------------------------------------//






//--------------- global functions ----------------//

String readMotorData(int motor_no){
  float posData = encoder[motor_no-1].getAngPos();

  String data = String(posData, 3);
  data += ",";
  data += String(filteredVel[motor_no-1], 4);
  data += ",";
  data += String(unfilteredVel[motor_no-1], 4);
  return data;
}


String readMotorPidVel(int motor_no)
{
  String data = String(target[motor_no-1], 4);
  data += ",";
  data += String(filteredVel[motor_no-1], 4);
  return data;
}


String writeMotorPWM(int motor_no, int pwm)
{
  if (pidMode[motor_no-1] == 0){
    motor[motor_no-1].sendPWM(pwm);
    cmdVelTimeout[motor_no-1] = millis();
    return "1";
  }
  else {
    return "0";
  }
}


String writeMotorSpeed(int motor_no, float targetVel)
{
  if (pidMode[motor_no-1] == 1){
    target[motor_no-1] = targetVel;
    cmdVelTimeout[motor_no-1] = millis();
    return "1";
  }
  else {
    return "0";
  }
}


String setPidModeFunc(int motor_no, int mode)
{
  pidMode[motor_no-1] = mode;

  motor[motor_no-1].sendPWM(0);
  pidMotor[motor_no-1].begin();

  return "1";
}
String getPidModeFunc(int motor_no)
{
  return String(pidMode[motor_no-1]);
}


String setEncoderPPR(int motor_no, float ppr)
{
  enc_ppr[motor_no-1] = ppr;
  encoder[motor_no-1].setPulsePerRev(enc_ppr[motor_no-1]);
  return "1";
}
String getEncoderPPR(int motor_no)
{
  return String(enc_ppr[motor_no-1]);
}


String setMotorKp(int motor_no, float Kp)
{
  kp[motor_no-1] = Kp;
  pidMotor[motor_no-1].setKp(kp[motor_no-1]);
  pidMotor[motor_no-1].begin();
  return "1";
}
String getMotorKp(int motor_no)
{
  return String(kp[motor_no-1], 4);
}


String setMotorKi(int motor_no, float Ki)
{
  ki[motor_no-1] = Ki;
  pidMotor[motor_no-1].setKi(ki[motor_no-1]);
  pidMotor[motor_no-1].begin();
  return "1";
}
String getMotorKi(int motor_no)
{
  return String(ki[motor_no-1], 4);
}


String setMotorKd(int motor_no, float Kd)
{
  kd[motor_no-1] = Kd;
  pidMotor[motor_no-1].setKd(kd[motor_no-1]);
  pidMotor[motor_no-1].begin();
  return "1";
}
String getMotorKd(int motor_no)
{
  return String(kd[motor_no-1], 4);
}


String setRdir(int motor_no, float dir)
{
  if (dir >= 0.0)
    rdir[motor_no-1] = 1.00;
  else
    rdir[motor_no-1] = -1.00;

  return "1";
}
String getRdir(int motor_no)
{
  return String(rdir[motor_no-1]);
}


String setCutoffFreq(int motor_no, float f0)
{
  cutOffFreq[motor_no-1] = f0;
  velFilter[motor_no-1].setCutOffFreq(cutOffFreq[motor_no-1]);
  return "1";
}
String getCutoffFreq(int motor_no)
{
  return String(cutOffFreq[motor_no-1]);
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
  for (int i=0; i<2; i+=1)
  {
    cmdVelTimeout[i] = millis();
  }
  return "1";
}
String getCmdTimeout()
{
  return String(cmdVelTimeoutInterval);
}


#endif