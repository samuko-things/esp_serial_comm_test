#ifndef SIMPLE_PID_CONTROL_H
#define SIMPLE_PID_CONTROL_H
#include <Arduino.h>


class SimplePID {
  public:
    SimplePID(double Kp, double Ki, double Kd, double out_min, double out_max);

    void setParameters(double Kp, double Ki, double Kd, double out_min, double out_max);
    void setGains(double Kp, double Ki, double Kd);
    void setKp(double Kp);
    void setKi(double Ki);
    void setKd(double Kd);
    void setOutLimit(double out_max, double out_min);
    void begin();
    double compute(double target, double actual);

  private:
    double error, errorPrev, errorInt, errorDot;
    unsigned long lastTime = 0;
    double kp, ki, kd;
    double outMax, outMin, outSat, outUnsat;
    bool outputIsClamped = false;
    bool integratorIsOn = true;

    void reset();
};




SimplePID::SimplePID(double Kp, double Ki, double Kd, double out_min, double out_max)
{
  reset();

  kp = Kp;
  ki = Ki;
  kd = Kd;
  outMax = out_max;
  outMin = out_min;

  errorPrev = error;
  lastTime = micros();
}

void SimplePID::setParameters(double Kp, double Ki, double Kd, double out_min, double out_max)
{
  kp = Kp;
  ki = Ki;
  kd = Kd;
  outMax = out_max;
  outMin = out_min;
}

void SimplePID::setGains(double Kp, double Ki, double Kd)
{
  kp = Kp;
  ki = Ki;
  kd = Kd;
}

void SimplePID::setKp(double Kp)
{
  kp = Kp;
}

void SimplePID::setKi(double Ki)
{
  ki = Ki;
}

void SimplePID::setKd(double Kd)
{
  kd = Kd;
}

void SimplePID::setOutLimit(double out_max, double out_min)
{
  outMax = out_max;
  outMin = out_min;
}

void SimplePID::begin()
{
  reset();
  lastTime = micros();
}

double SimplePID::compute(double target, double actual)
{
  double now = micros();
  double dt = (now - lastTime) / 1.0e6;
  if (dt <= 0.0) dt = 1e-6; // prevent div by zero

  error = target - actual;

  // Integral term
  if (integratorIsOn)
      errorInt += error * dt;

  // Derivative term with simple low-pass filter
  errorDot = (error - errorPrev) / dt;
  // double derivative = (error - errorPrev) / dt;
  // double alpha = 0.25; // tune between 0-1
  // errorDot = alpha * derivative + (1 - alpha) * errorDot;

  // PID output
  outUnsat = (kp * error) + (ki * errorInt) + (kd * errorDot);

  // Output saturation
  if (outUnsat > outMax) {
      outSat = outMax;
      outputIsClamped = true;
  } else if (outUnsat < outMin) {
      outSat = outMin;
      outputIsClamped = true;
  } else {
      outSat = outUnsat;
      outputIsClamped = false;
  }

  // Anti-windup
  integratorIsOn = !outputIsClamped;

  errorPrev = error;
  lastTime = now;

  return outSat;
}

void SimplePID::reset()
{
  error = 0.0;
  errorPrev = 0.0;
  errorInt = 0.0;
  errorDot = 0.0;
  outSat = 0.0;
  outUnsat = 0.0;
  lastTime = 0;
}


#endif
