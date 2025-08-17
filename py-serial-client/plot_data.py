import time
from esp32_serial_client import Esp32SerialServer

port = '/dev/ttyUSB0'
motorController = Esp32SerialServer(port)

if __name__ == '__main__':
  
  motorController.setParam("/timeout", -1, 5000)
  res = motorController.getParam("/timeout", -1)
  print('cmd_timeout: ', res)

  res = motorController.getParam("/kp", 1)
  print('kp: ', res)
  res = motorController.getParam("/ki", 1)
  print('ki: ', res)
  res = motorController.getParam("/kd", 1)
  print('kd: ', res)
  res = motorController.getParam("/max-vel", 1)
  print('maxVel: ', res)

  res = motorController.writeSpeed(1, 3.0)
  print(res)
  # # time.sleep(2.0)
  # res = motorController.writeSpeed(1, 3.142)
  # print(res)

  # res = motorController.writePWM(0, 70)
  # print(res)
  # res = motorController.writePWM(1, 100)
  # print(res)

  time.sleep(7.5)

  res = motorController.writeSpeed(1, 0.00)
  print(res)
  # res = motorController.writeSpeed(1, 0.00)
  # print(res)