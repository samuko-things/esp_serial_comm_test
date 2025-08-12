import serial
import time

class Esp32SerialServer:
    def __init__(self, port, baud=115200, timeOut=0.1):
        self.ser = serial.Serial(port, baud, timeout=timeOut)
    
    def send_and_receive(self, msg_cmd):
        data = ""
        prev_time = time.time()
        while data=="":
            try:
                self.ser.write(msg_cmd.encode())   # send a single or multiple byte    
                data = self.ser.readline().decode().strip()
                if time.time()-prev_time > 2.0:
                    raise Exception("[Timeout] No response from ESP32")
            except:
                print("[Timeout] No response from ESP32")
        return data
    
    def getMotorAData(self):
        '''
        returns motorA rotational angularPosition (in rad) and angularVelocity (in rad/sec)
        '''
        data = self.send_and_receive("/motorA-data").split(',')
        return float(data[0]), float(data[1])
    
    def getMotorBData(self):
        '''
        returns motorB rotational angularPosition (in rad) and angularVelocity (in rad/sec)
        '''
        data = self.send_and_receive("/motorB-data").split(',')
        return float(data[0]), float(data[1])
    
    def getMotorA_PID_vel(self):
        '''
        returns motorA target and actual angular velocities respectively
        '''
        data = self.send_and_receive("/motorA-pid-vel").split(',')
        return float(data[0]), float(data[1])
    
    def getMotorB_PID_vel(self):
        '''
        returns motorB target and actual angular velocities respectively
        '''
        data = self.send_and_receive("/motorB-pid-vel").split(',')
        return float(data[0]), float(data[1])
    
    def cmdMotorAPWM(self, pwm):
        '''
        send PWM command to motorA and returns the PWM command received
        '''
        cmd_route = "/motorA-pwm" + "," + str(pwm)
        data = self.send_and_receive(cmd_route).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def cmdMotorBPWM(self, pwm):
        '''
        send PWM command to motorB and returns the PWM command received
        '''
        cmd_route = "/motorB-pwm" + "," + str(pwm)
        data = self.send_and_receive(cmd_route).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def cmdMotorAVel(self, targetVel):
        '''
        send target velocity (in rad/sec) command to motorA and returns the target Velocity command received
        '''
        cmd_route = "/motorA-vel" + "," + str(targetVel)
        data = self.send_and_receive(cmd_route).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def cmdMotorBVel(self, targetVel):
        '''
        send target velocity (in rad/sec) command to motorB and returns the target Velocity command received
        '''
        cmd_route = "/motorB-vel" + "," + str(targetVel)
        data = self.send_and_receive(cmd_route).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def setParam(self, cmd_route, val1=0):
        cmd_str = cmd_route+","+str(val1)
        data = self.send_and_receive(cmd_str).split(',')
        if data[0] == "1":
            return True
        else:
            return False
  
  
    def getParam(self, cmd_route):
        data = self.send_and_receive(cmd_route).split(',')
        return float(data[0])
    

    


# if __name__ == "__main__":
#     client = Esp32SerialServer('/dev/ttyUSB0')

#     comm_freq = 50.0 # Hz
#     comm_period = 1.0 / comm_freq

#     while True:
#         start_time = time.time()

#         msg = client.send_and_receive("/sensor")
#         print(msg)

#         elapsed = time.time() - start_time
#         sleep_time = max(0, comm_period - elapsed)
#         time.sleep(sleep_time)