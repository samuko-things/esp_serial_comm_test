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

      if (dataMsgBufferArray[0] == "/motorA-data")
      {
        sendMsg = readMotorAData();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorB-data")
      {
        sendMsg = readMotorBData();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorA-pid-vel")
      {
        sendMsg = readMotorA_PID_vel();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorB-pid-vel")
      {
        sendMsg = readMotorB_PID_vel();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorA-pwm")
      {
        sendMsg = cmdMotorAPWM(dataMsgBufferArray[1].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorB-pwm")
      {
        sendMsg = cmdMotorBPWM(dataMsgBufferArray[1].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorA-vel")
      {
        sendMsg = cmdMotorAVel(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/motorB-vel")
      {
        sendMsg = cmdMotorBVel(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/mode")
      {
        sendMsg = setPidModeFunc(dataMsgBufferArray[1].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pprA")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendEncAppr();
        else
          sendMsg = setEncAppr(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pprB")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendEncBppr();
        else
          sendMsg = setEncBppr(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kpA")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorAkp();
        else
          sendMsg = setMotorAkp(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kpB")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorBkp();
        else
          sendMsg = setMotorBkp(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kiA")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorAki();
        else
          sendMsg = setMotorAki(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kiB")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorBki();
        else
          sendMsg = setMotorBki(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kdA")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorAkd();
        else
          sendMsg = setMotorAkd(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kdB")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendMotorBkd();
        else
          sendMsg = setMotorBkd(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rdirA")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendRdirA();
        else
          sendMsg = setRdirA(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rdirB")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendRdirB();
        else
          sendMsg = setRdirB(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/f0A")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendVelFilterCutoffFreqA();
        else
          sendMsg = setVelFilterCutoffFreqA(dataMsgBufferArray[1].toFloat());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/f0B")
      {
        if (dataMsgBufferArray[1] == "")
          sendMsg = sendVelFilterCutoffFreqB();
        else
          sendMsg = setVelFilterCutoffFreqB(dataMsgBufferArray[1].toFloat());
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