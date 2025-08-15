import time
from esp32_serial_client import Esp32SerialServer

if __name__ == '__main__':
  port = '/dev/ttyUSB0'
  motorController = Esp32SerialServer(port)
  
  motorController.setParam("/timeout", -1, 5000)
  res = motorController.getParam("/timeout", -1)
  print('cmd_timeout: ', res)

  # res = motorController.writeSpeed(0, 3.142)
  # print(res)
  # # time.sleep(2.0)
  # res = motorController.writeSpeed(1, 3.142)
  # print(res)
  # motorController.writeSpeed(0, 3.142)
  # motorController.writeSpeed(0, 3.142)
  # motorController.writeSpeed(0, 3.142)
  # motorController.writeSpeed(0, 3.142)
  # motorController.writeSpeed(0, 3.142)
  res = motorController.writeSpeed(1, 3.142)

  print(res)
  # # time.sleep(2.0)
  # res = motorController.writeSpeed(1, 3.142)
  # print(res)

  # res = motorController.writePWM(0, 70)
  # print(res)
  # res = motorController.writePWM(1, 100)
  # print(res)

  time.sleep(5.0)

  res = motorController.writeSpeed(0, 0.00)
  print(res)
  # res = motorController.writeSpeed(1, 0.00)
  # print(res)