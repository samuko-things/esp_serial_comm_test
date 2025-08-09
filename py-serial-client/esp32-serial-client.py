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
    


if __name__ == "__main__":
    client = Esp32SerialServer('/dev/ttyUSB0')

    comm_freq = 50.0 # Hz
    comm_period = 1.0 / comm_freq

    while True:
        start_time = time.time()

        msg = client.send_and_receive("/sensor")
        print(msg)

        elapsed = time.time() - start_time
        sleep_time = max(0, comm_period - elapsed)
        time.sleep(sleep_time)