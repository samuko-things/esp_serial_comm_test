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
    
    def readData(self, motor_no):
        '''
        returns rotational angularPosition (in rad), filtered angularVelocity (in rad/sec), unfiltered angularVelocity (in rad/sec)
        '''
        cmd_str = "/data" + "," + str(motor_no)
        data = self.send_and_receive(cmd_str).split(',')
        return float(data[0]), float(data[1]), float(data[2])
    
    def readPidVel(self, motor_no):
        '''
        returns target angularVelocity (in rad/sec), filtered (i.e actual) angularVelocity (in rad/sec)
        '''
        cmd_str = "/pvel" + "," + str(motor_no)
        data = self.send_and_receive(cmd_str).split(',')
        return float(data[0]), float(data[1])
    
    def writePWM(self, motor_no, pwm):
        cmd_str = "/pwm" + "," + str(motor_no) + "," + str(pwm)
        data = self.send_and_receive(cmd_str).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def writeSpeed(self, motor_no, speed):
        cmd_str = "/vel" + "," + str(motor_no) + "," + str(speed)
        data = self.send_and_receive(cmd_str).split(',')
        if data[0] == "1":
            return True
        else:
            return False
    
    def setParam(self, cmd_route, motor_no=0, param_val=0):
        cmd_str = cmd_route + "," + str(motor_no) + "," + str(param_val)
        data = self.send_and_receive(cmd_str).split(',')
        return data[0]
        # if data[0] == "1":
        #     return True
        # else:
        #     return False
  
    def getParam(self, cmd_route, motor_no=0):
        cmd_str = cmd_route + "," + str(motor_no)
        data = self.send_and_receive(cmd_str).split(',')
        return float(data[0])