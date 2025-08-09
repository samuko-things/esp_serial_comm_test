#include <Arduino.h>
#include "serial_comm.h"

//set up to connect to an existing network (e.g. mobile hotspot from laptop that will run the python code)
int counter = 0;


unsigned long sensorUpdateTime, sensorUpdateTimeInterval=50;
unsigned long serialLoopTime, serialLoopTimeInterval=5;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  serialLoopTime = millis();
  sensorUpdateTime = millis();
}

void loop()
{  

  if ((millis() - sensorUpdateTime) >= sensorUpdateTimeInterval)
  {
    // Serial.println("Reading Sensors");
    sensorA = -1 * (float)random(10000, 10000000) / 100.0;
    sensorB = -1 * (float)random(10000, 10000000) / 100.0;
    sensorC = -1 * (float)random(10000, 10000000) / 100.0;

    sensorUpdateTime = millis();
  }
  
  if ((millis() - serialLoopTime) >= serialLoopTimeInterval)
  {
    recieve_and_send_data();
    serialLoopTime = millis();
  }
  
}