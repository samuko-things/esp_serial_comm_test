import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
from esp32_serial_client import Esp32SerialServer




def animate(i):
    global motorController, axes, dataList1, dataList2, dataPoints

    posA, velA, _ = motorController.readData()
    posB, velB, _ = motorController.readData()

    dataList1.append(velA)
    dataList2.append(velB)

    # Fix the list size so that the animation plot 'window' is x number of points
    dataList1 = dataList1[-dataPoints:]
    dataList2 = dataList2[-dataPoints:]
    
    axes.clear()
    axes.plot(dataList1)
    axes.plot(dataList2)
    
    axes.grid(which = "major", linewidth = 0.5)
    axes.grid(which = "minor", linewidth = 0.2)
    axes.minorticks_on()

    axes.set_ylim([-30, 30]) # Set Y axis limit of plot
    axes.set_title("Motor Data") # Set title of figure
    axes.set_ylabel("angular pos (radians)") # Set title of y axis 
    axes.set_xlabel("number of data points") # Set title of z axis 

    axes.legend(["data_list_1", "data_list_2"], loc ="upper right")






port = '/dev/ttyUSB0'
motorController = Esp32SerialServer(port)

dataList1 = []
dataList2 = []

dataPoints = 50
                                                        
fig = plt.figure()  # Create Matplotlib plots fig is the 'higher level' plot window
axes = fig.add_subplot(111) # Add subplot to main fig window

if __name__ == '__main__':
  # res = motorController.setParam("/cmd-timeout", 0, 4000)
  # print(res)

  pidMode = 1
  # res = motorController.setParam("/mode", 1, pidMode)
  # print(res)
  # res = motorController.getParam("/mode", 1)
  # print('mode_1: ', res)
  # motorController.setParam("/mode", 2, pidMode)
  # res = motorController.getParam("/mode", 2)
  # print('mode_2: ', res)

  # print()


  motorController.setParam("/ppr", 1, 374.25)
  res = motorController.getParam("/ppr", 1)
  print('ppr_1: ', res)
  motorController.setParam("/ppr", 2, 1092.0)
  res = motorController.getParam("/ppr", 2)
  print('ppr_2: ', res)

  print()

  motorController.setParam("/kp", 1, 35.0)
  res = motorController.getParam("/kp", 1)
  print('kp_1: ', res)
  motorController.setParam("/ki", 1, 70.0)
  res = motorController.getParam("/ki", 1)
  print('ki_1: ', res)
  motorController.setParam("/kd", 1, 3.5)
  res = motorController.getParam("/kd", 1)
  print('kd_1: ', res)

  print()

  motorController.setParam("/kp", 2, 80.0)
  res = motorController.getParam("/kp", 2)
  print('kp_2: ', res)
  motorController.setParam("/ki", 2, 160.0)
  res = motorController.getParam("/ki", 2)
  print('ki_2: ', res)
  motorController.setParam("/kd", 2, 8.0)
  res = motorController.getParam("/kd", 2)
  print('kd_2: ', res)

  print()

  if pidMode == 1:
    res = motorController.writeSpeed(1, 6.284)
    print(res)
    time.sleep(2.0)
    res = motorController.writeSpeed(2, 6.284)
    print(res)

  

  if pidMode == 0:
    res = motorController.writePWM(1, 70)
    print(res)
    res = motorController.writePWM(2, 100)
    print(res)

  # time.sleep(10.0)

  # if pidMode == 1:
  #   res = motorController.writeSpeed(1, 0.00)
  #   print(res)
  #   res = motorController.writeSpeed(2, 0.00)
  #   print(res)

  # if pidMode == 0:
  #   res = motorController.writePWM(1, 0)
  #   print(res)
  #   res = motorController.writePWM(2, 0)
  #   print(res)



  # time.sleep(2.0)

  # while True:
  #   pos_1, vel_1, vel0_1 = motorController.readData(1)
  #   pos_2, vel_2, vel0_2 = motorController.readData(2)

  #   print(pos_1, vel_1, vel0_1)
  #   print(pos_2, vel_2, vel0_2)
  #   print()

  #   time.sleep(0.1)

  # ani = animation.FuncAnimation(fig, animate, frames=100, interval=50)
  # plt.show()