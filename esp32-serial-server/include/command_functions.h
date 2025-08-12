#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>
#include "l298n_motor_control.h"
#include "encoder_setup.h"
#include "adaptive_low_pass_filter.h"
#include "simple_pid_control.h"

//--------------- global variables -----------------//
int ledState = 0;
float sensorA, sensorB, sensorC;

// motor A H-Bridge Connection
int IN1 = 26, IN2 = 27, enA = 25;
L298NMotorControl motorA(IN1, IN2, enA);

// motor B H-Bridge Connection
int IN3 = 14, IN4 = 12, enB = 13;
L298NMotorControl motorB(IN3, IN4, enB);

// store encoder pulsePerRev needed by encoder
float encA_ppr = 374.25;
float encB_ppr = 374.25;

int encA_clkPin = 18, encA_dirPin = 19; // encA_ppr parameter is decleared globally in the global_params_eeprom.h file.
int encB_clkPin = 16, encB_dirPin = 17; // encB_ppr parameter is decleared globally in the global_params_eeprom.h file.

QuadEncoder encA(encA_clkPin, encA_dirPin, encA_ppr);
QuadEncoder encB(encB_clkPin, encB_dirPin, encB_ppr);

// adaptive lowpass Filter
float velFilterCutOffFreqA = 1.0;
float velFilterCutOffFreqB = 1.0;

// Filter instance
AdaptiveLowPassFilter velFilterA(1, velFilterCutOffFreqA);
AdaptiveLowPassFilter velFilterB(1, velFilterCutOffFreqB);

float filteredVelA=0.0, filteredVelB=0.0;
float unfilteredVelA=0.0, unfilteredVelB=0.0;

float outMin = -255.0, outMax = 255.0;

// motorA pid control global params needed by pid
float kpA = 0.0;
float kiA = 0.0;
float kdA = 0.0;
float targetA = 0.00;
float outputA;

// motorB pid control global params needed by pid
float kpB = 0.0;
float kiB = 0.0;
float kdB = 0.0;
float targetB = 0.00;
float outputB;

// motorA pid control
SimplePID pidMotorA(kpA, kiA, kdA, outMin, outMax);

// motorA pid control
SimplePID pidMotorB(kpB, kiB, kdB, outMin, outMax);

// check if in PID or PWM mode
bool pidMode = false; // true-PID MODE, false-SETUP MODE

float rdirA = 1.00;
float rdirB = 1.00;

// calcute allowable maximum angular velocity (for overall smooth operation of the whole system)
float freq_per_tick_allowable = 2000.0; // Hz
float wA_allowable = 10.00;
float wB_allowable = 10.00;

float calc_wA_allowable()
{
  wA_allowable = (2 * PI * freq_per_tick_allowable) / encA_ppr;
  return wA_allowable;
}

float calc_wB_allowable()
{
  wB_allowable = (2 * PI * freq_per_tick_allowable) / encB_ppr;
  return wB_allowable;
}

// maximum motor velocity that can be commanded
float maxVelA = calc_wA_allowable(); // in radians/sec
float maxVelB = calc_wB_allowable(); // in radians/sec

// for command timeout.
unsigned long cmdVelTimeout, cmdVelTimeoutSampleTime = 0; // ms -> (1000/sampleTime) hz
//-------------------------------------------------//

//--------------- global functions ----------------//
String readMotorAData()
{
  float posData = encA.getAngPos();
  float velData = encA.getAngVel();

  String data = String(posData, 3);
  data += ",";
  data += String(velData, 4);
  return data;
}

String readMotorBData()
{
  float posData = encB.getAngPos();
  float velData = encB.getAngVel();

  String data = String(posData, 3);
  data += ",";
  data += String(velData, 4);
  return data;
}

String readMotorA_PID_vel()
{
  // float actualVel = encA.getAngVel();

  String data = String(targetA, 3);
  data += ",";
  data += String(filteredVelA, 4);
  return data;
}

String readMotorB_PID_vel()
{
  // float actualVel = encB.getAngVel();

  String data = String(targetB, 3);
  data += ",";
  data += String(filteredVelB, 4);
  return data;
}

String cmdMotorAPWM(int pwm)
{
  motorA.sendPWM(pwm);
  return "1";
}

String cmdMotorBPWM(int pwm)
{
  motorB.sendPWM(pwm);
  return "1";
}

String cmdMotorAVel(float targetVel)
{
  targetA = targetVel;
  return "1";
}

String cmdMotorBVel(float targetVel)
{
  targetB = targetVel;
  return "1";
}


String setPidModeFunc(int mode)
{
  if (mode == 0)
  {
    pidMode = false;
    motorA.sendPWM(0);
    motorB.sendPWM(0);
    pidMotorA.begin();
    pidMotorB.begin();
  }
  else if (mode == 1)
  {
    pidMode = true;
    motorA.sendPWM(0);
    motorB.sendPWM(0);
    pidMotorA.begin();
    pidMotorB.begin();
  }
  return "1";
}



String setEncAppr(float ppr)
{
  // setPPR_A(ppr);
  // encA_ppr = getPPR_A();
  encA_ppr = ppr;
  encA.setPulsePerRev(encA_ppr);
  return "1";
}
String sendEncAppr()
{
  return String(encA_ppr);
}



String setEncBppr(float ppr)
{
  encB_ppr = ppr;
  encB.setPulsePerRev(encB_ppr);
  return "1";
}
String sendEncBppr()
{
  return String(encB_ppr);
}



String setMotorAkp(float kp)
{
  kpA = kp;
  pidMotorA.setKp(kpA);
  pidMotorA.begin();
  return "1";
}
String sendMotorAkp()
{
  return String(kpA, 4);
}



String setMotorBkp(float kp)
{
  kpB = kp;
  pidMotorB.setKp(kpB);
  pidMotorB.begin();
  return "1";
}
String sendMotorBkp()
{
  return String(kpB, 4);
}



String setMotorAki(float ki)
{
  kiA = ki;
  pidMotorA.setKi(kiA);
  pidMotorA.begin();
  return "1";
}
String sendMotorAki()
{
  return String(kiA, 4);
}



String setMotorBki(float ki)
{
  kiB = ki;
  pidMotorB.setKi(kiB);
  pidMotorB.begin();
  return "1";
}
String sendMotorBki()
{
  return String(kiB, 4);
}



String setMotorAkd(float kd)
{
  kdA = kd;
  pidMotorA.setKd(kdA);
  pidMotorA.begin();
  return "1";
}
String sendMotorAkd()
{
  return String(kdA, 4);
}



String setMotorBkd(float kd)
{
  kdB = kd;
  pidMotorB.setKd(kdB);
  pidMotorB.begin();
  return "1";
}
String sendMotorBkd()
{
  return String(kdB, 4);
}



String setRdirA(float dir)
{
  if (dir >= 0.0)
  {
    rdirA = 1.00;
  }
  else
  {
    rdirA = -1.00;
  }
  return "1";
}
String sendRdirA()
{
  return String(rdirA);
}




String setRdirB(float dir)
{
  if (dir >= 0.0)
  {
    rdirA = 1.00;
  }
  else
  {
    rdirA = -1.00;
  }
  return "1";
}
String sendRdirB()
{
  return String(rdirB);
}



String setVelFilterCutoffFreqA(float f0)
{
  velFilterCutOffFreqA = f0;
  velFilterA.setCutOffFreq(velFilterCutOffFreqA);
  return "1";
}
String sendVelFilterCutoffFreqA()
{
  return String(velFilterCutOffFreqA);
}



String setVelFilterCutoffFreqB(float f0)
{
  velFilterCutOffFreqB = f0;
  velFilterB.setCutOffFreq(velFilterCutOffFreqB);
  return "1";
}
String sendVelFilterCutoffFreqB()
{
  return String(velFilterCutOffFreqB);
}



#endif