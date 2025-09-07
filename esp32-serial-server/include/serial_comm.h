#include <Arduino.h>
#include "command_functions.h"

// Command IDs
const uint8_t START_BYTE = 0xAA;
const uint8_t READ_SENSOR = 0x01;
const uint8_t CMD_LED = 0x02;

void processCommand(uint8_t cmd, uint8_t* data, uint8_t len) {
  int pos = (int)data[0];
  float value;
  memcpy(&value, &data[1], sizeof(float));

  switch (cmd) {
    case READ_SENSOR: {
      float data = readSensorData(pos);
      Serial.write((uint8_t*)&data, sizeof(data));
      break;
    }

    case CMD_LED: {
      float res = commandLed((int)value);
      Serial.write((uint8_t*)&res, sizeof(res));
      break;
    }

    default:
      float error = 0.0;
      Serial.write((uint8_t*)&error, sizeof(error));
  }
}

// void sendData(uint8_t CMD_VAL, float value) {
//   uint8_t payload[4];
//   memcpy(payload, &value, sizeof(float));  // float → 4 raw bytes

//   uint8_t length = sizeof(float);
//   uint8_t checksum = START_BYTE + CMD_VAL + length;

//   Serial.write(START_BYTE);
//   Serial.write(CMD_VAL);
//   Serial.write(length);
//   for (int i = 0; i < length; i++) {
//     Serial.write(payload[i]);
//     checksum += payload[i];
//   }
//   Serial.write(checksum & 0xFF);

//   // float f = 3.14;
//   // Serial.write((uint8_t*)&f, sizeof(f));
// }


void recieve_and_send_data() {
  static uint8_t state = 0;
  static uint8_t cmd, length;
  static uint8_t buffer[32];
  static uint8_t index = 0;
  static uint8_t checksum = 0;

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
        index = 0;
        state = 3;
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
