#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include "command_functions.h"

String dataMsg = "", dataMsgBuffer, dataMsgBufferArray[3];
String sendMsg = "";


void recieve_and_send_data(){
  int indexPos = 0, i = 0;

  if (Serial.available() > 0)
  {
    while (Serial.available())
    {
      dataMsg = Serial.readString();
    }
    dataMsg.trim();
    if (dataMsg != "")
    {
      do
      {
        indexPos = dataMsg.indexOf(',');
        if (indexPos != -1)
        {
          dataMsgBuffer = dataMsg.substring(0, indexPos);
          dataMsg = dataMsg.substring(indexPos + 1, dataMsg.length());
          dataMsgBufferArray[i] = dataMsgBuffer;
          dataMsgBuffer = "";
        }
        else
        {
          if (dataMsg.length() > 0)
            dataMsgBufferArray[i] = dataMsg;
        }
        i += 1;
      } while (indexPos >= 0);
    }


    if (dataMsgBufferArray[0] != "")
    {
      digitalWrite(LED_BUILTIN, HIGH);

      if (dataMsgBufferArray[0] == "/led")
      {
        sendMsg = commandLed(dataMsgBufferArray[1].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/sensor")
      {
        sendMsg = readSensorData(dataMsgBufferArray[0]);
        Serial.println(sendMsg);
      }

      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);

      sendMsg = "0";
      Serial.println(sendMsg);

      digitalWrite(LED_BUILTIN, LOW);
    }

    sendMsg = "";
    dataMsg = "";
    dataMsgBuffer = "";
    dataMsgBufferArray[0] = "";
    dataMsgBufferArray[1] = "";
    dataMsgBufferArray[2] = "";
  } 
}

#endif