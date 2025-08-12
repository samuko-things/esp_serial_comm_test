import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
from esp32_serial_client import Esp32SerialServer




def animate(i):
    global client, axes, dataList1, dataList2, dataPoints

    posA, velA = client.getMotorAData()
    posB, velB = client.getMotorBData()

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
client = Esp32SerialServer(port)

dataList1 = []
dataList2 = []

dataPoints = 50
                                                        
fig = plt.figure()  # Create Matplotlib plots fig is the 'higher level' plot window
axes = fig.add_subplot(111) # Add subplot to main fig window

if __name__ == '__main__':
  # client.cmdMotorAPWM(80)
  # client.cmdMotorBPWM(-80)
  client.setParam("/mode", 0)
  # velA = client.cmdMotorAVel(0.0)
  velB = client.cmdMotorBVel(6.284)
  print(velB)
  # ani = animation.FuncAnimation(fig, animate, frames=100, interval=50)
  # plt.show()