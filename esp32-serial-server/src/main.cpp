#include <Arduino.h>
#include "serial_comm.h"
#include "l298n_motor_control.h"
#include "encoder_setup.h"

// motor A H-Bridge Connection
int IN1 = 26, IN2 = 27, enA = 25;
L298NMotorControl motorA(IN1, IN2, enA);

// motor B H-Bridge Connection
int IN3 = 14, IN4 = 12, enB = 13;
L298NMotorControl motorB(IN3, IN4, enB);


///////////////////////////////////////////////////
// store encoder pulsePerRev needed by encoder
float encA_ppr = 374.25;
float encB_ppr = 374.25;

int encA_clkPin = 18, encA_dirPin = 19; // encA_ppr parameter is decleared globally in the global_params_eeprom.h file.
int encB_clkPin = 16, encB_dirPin = 17; // encB_ppr parameter is decleared globally in the global_params_eeprom.h file.

QuadEncoder encA(encA_clkPin, encA_dirPin, encA_ppr);
QuadEncoder encB(encB_clkPin, encB_dirPin, encB_ppr);


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


unsigned long sensorUpdateTime, sensorUpdateTimeInterval=500;
unsigned long serialLoopTime, serialLoopTimeInterval=5;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  delay(10000);
  encoderInit();

  motorA.sendPWM(100);
  motorB.sendPWM(100);


  serialLoopTime = millis();
  sensorUpdateTime = millis();
}

void loop()
{
  if ((millis() - sensorUpdateTime) >= sensorUpdateTimeInterval)
  {
    // Serial.println("Reading Sensors");
    // sensorA = -1 * (float)random(10000, 10000000) / 100.0;
    // sensorB = -1 * (float)random(10000, 10000000) / 100.0;
    // sensorC = -1 * (float)random(10000, 10000000) / 100.0;

    Serial.println(encA.getAngVel());
    Serial.println(encB.getAngVel());
    Serial.println();

    sensorUpdateTime = millis();
  }
  
  // if ((millis() - serialLoopTime) >= serialLoopTimeInterval)
  // {
  //   recieve_and_send_data();
  //   serialLoopTime = millis();
  // }
  
}