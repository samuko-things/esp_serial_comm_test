#include <Arduino.h>
#include "command_functions.h"

// Command IDs
const uint8_t START_BYTE = 0xAA;
const uint8_t READ_SENSOR = 0x01;
const uint8_t CMD_LED = 0x02;
const uint8_t CMD_MOTOR = 0x03;

void processCommand(uint8_t cmd, uint8_t* data, uint8_t length) {

  switch (cmd) {
    case READ_SENSOR: {
      Serial.write((uint8_t*)&a, sizeof(a));
      Serial.write((uint8_t*)&b, sizeof(b));
      Serial.write((uint8_t*)&c, sizeof(c));
      Serial.write((uint8_t*)&d, sizeof(d));
      break;
    }

    case CMD_LED: {
      uint8_t pos = data[0];
      float value;
      memcpy(&value, &data[1], sizeof(float));
      float res = commandLed((int)value);
      Serial.write((uint8_t*)&res, sizeof(res));
      break;
    }

    case CMD_MOTOR: {
      float w, x, y, z;
      memcpy(&w, &data[0], sizeof(float));
      memcpy(&x, &data[4], sizeof(float));
      memcpy(&y, &data[8], sizeof(float));
      memcpy(&z, &data[12], sizeof(float));
      // store values in necessary places
      float res = 1.0;
      Serial.write((uint8_t*)&res, sizeof(res));
      break;
    }

    default:
      float error = 0.0;
      Serial.write((uint8_t*)&error, sizeof(error));
      break;
  }
}


void recieve_and_send_data() {
  static uint8_t state = 0;
  static uint8_t cmd, length;
  static uint8_t buffer[32];
  static uint8_t index = 0;
  static uint8_t checksum = 0, tempCheck;

  while (Serial.available()) {
    uint8_t b = Serial.read();

    switch (state) {
      case 0: // Wait for start
        if (b == START_BYTE) {
          state = 1;
          checksum = b;
        }
        break;

      case 1: // Command
        cmd = b;
        checksum += b;
        state = 2;
        break;

      case 2: // Length
        length = b;
        checksum += b;
        if (length==0){
          state = 4;
        }
        else{
          index = 0;
          state = 3;
        }
        break;

      case 3: // Payload
        buffer[index++] = b;
        checksum += b;
        if (index >= length) state = 4;
        break;

      case 4: // Checksum
        if ((checksum & 0xFF) == b) {
          processCommand(cmd, buffer, length);
        } else {
          float error = 0.0;
          Serial.write((uint8_t*)&error, sizeof(error));
        }
        state = 0; // reset for next packet
        break;
    }
  }
}
