#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>

//--------------- global variables -----------------//
int ledState = 0;
float sensorA, sensorB, sensorC;
//-------------------------------------------------//


//--------------- global functions ----------------//
String commandLed(int state){
  ledState = state;
  // digitalWrite(LED_BUILTIN, ledState);
  return String(ledState);
}

String readSensorData(String cmd_route){
  String data = String(sensorA, 4);
  data += ",";
  data += String(sensorB, 4);
  data += ",";
  data += String(sensorC, 4);
  return data;
}
//-------------------------------------------------//

#endif