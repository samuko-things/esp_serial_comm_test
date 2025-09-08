#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>

//--------------- global variables -----------------//
int ledState = 0;
float a, b, c, d;
//-------------------------------------------------//


//--------------- global functions ----------------//
int commandLed(int state){
  digitalWrite(LED_BUILTIN, state);
  return 1.0;
}

// float readSensorData(int pos){
//   return sensor[pos];
// }
//-------------------------------------------------//

#endif