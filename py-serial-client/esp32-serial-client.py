import serial
import struct
import time

# ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.1)
# ser = serial.Serial('/dev/ttyUSB0', 460800, timeout=0.1)
ser = serial.Serial('/dev/ttyUSB0', 921600, timeout=0.1)

START_BYTE = 0xAA
READ_SENSOR = 0x01
CMD_LED = 0x02
CMD_MOTOR = 0x03


#------------------------------------------------------------------------
def send_packet(cmd):
    length = 0
    packet = bytearray([START_BYTE, cmd, length])
    checksum = sum(packet) & 0xFF
    packet.append(checksum)
    ser.write(packet)

def send_packet_stream(cmd, payload_bytes):
    length = len(payload_bytes)
    packet = bytearray([START_BYTE, cmd, length]) + payload_bytes
    checksum = sum(packet) & 0xFF
    packet.append(checksum)
    ser.write(packet)

def read_packet():
    payload = ser.read(4)
    a = struct.unpack('<f', payload)[0]  # little-endian float
    return a

def read_packet_stream():
    payload = ser.read(16)
    a, b, c, d = struct.unpack('<ffff', payload)  # little-endian float
    return a, b, c, d
#---------------------------------------------------------------------


def write_data(cmd, pos, val):
    payload = struct.pack('<Bf', pos, val)
    send_packet_stream(cmd, payload)
    val = read_packet()
    return val

def read_data(cmd):
    payload = struct.pack('<f', 0.0)  # big-endian
    send_packet(cmd, payload)
    val = read_packet()
    return val

def write_data_stream(cmd, a, b, c, d):
    payload = struct.pack('<ffff', a,b,c,d) 
    send_packet_stream(cmd, payload)
    val = read_packet()
    return val

def read_data_stream(cmd):
    send_packet(cmd)
    a, b, c, d = read_packet_stream()
    return a, b, c, d

# Example usage
state=0

while True:
    # time.sleep(0.1)
    start_time = time.time()
    if state == 1:
        state = 0
    else:
        state = 1
    # res = write_data(CMD_LED, 255, state)
    # print(f'commanded LED STATE: {res}')
    w = 1.689
    x = 3.678
    y = 9.3456
    z = 45.90

    res = write_data_stream(CMD_MOTOR, w, x, y, z)
    res = write_data_stream(CMD_MOTOR, w, x, y, z)
    # print(f'commanded MOTOR: {res}')

    a, b, c, d = read_data_stream(READ_SENSOR)
    a, b, c, d = read_data_stream(READ_SENSOR)
    a, b, c, d = read_data_stream(READ_SENSOR)

    # print()
    end_time = time.time()
    print(end_time-start_time)