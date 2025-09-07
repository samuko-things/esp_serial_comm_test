import serial
import struct
import time

ser = serial.Serial('/dev/ttyUSB0', 921600, timeout=0.1)

START_BYTE = 0xAA
READ_SENSOR = 0x01
CMD_LED = 0x02

def send_packet(cmd, payload_bytes):
    length = len(payload_bytes)
    packet = bytearray([START_BYTE, cmd, length]) + payload_bytes
    checksum = sum(packet) & 0xFF
    # print(checksum)
    packet.append(checksum)
    ser.write(packet)


def read_packet():
    payload = ser.read(4)
    value = struct.unpack('<f', payload)[0]  # little-endian float
    return value


def write(cmd, pos, value):
    payload = struct.pack('<Bf', pos, value)  # big-endian
    send_packet(cmd, payload)
    val = read_packet()
    return val

def read(cmd, pos):
    payload = struct.pack('<Bf', pos, 0.0)  # big-endian
    send_packet(cmd, payload)
    val = read_packet()
    return val

# Example usage
state=0

while True:
    start_time = time.time()
    if state == 1:
        state = 0
    else:
        state = 1
    res = write(CMD_LED, 0, state)
    # print(f'commanded LED STATE: {res}')

    sensor0 = read(READ_SENSOR, 0)
    # print(f'sensor0: {sensor0}')
    sensor1 = read(READ_SENSOR, 1)
    # print(f'sensor1: {sensor1}')
    sensor2 = read(READ_SENSOR, 2)
    # print(f'sensor2: {sensor2}')

    # sensor0 = read(READ_SENSOR, 0)
    # # print(f'sensor0: {sensor0}')
    # sensor1 = read(READ_SENSOR, 1)
    # # print(f'sensor1: {sensor1}')
    # sensor2 = read(READ_SENSOR, 2)
    # # print(f'sensor2: {sensor2}')

    # sensor0 = read(READ_SENSOR, 0)
    # # print(f'sensor0: {sensor0}')
    # sensor1 = read(READ_SENSOR, 1)
    # # print(f'sensor1: {sensor1}')
    # sensor2 = read(READ_SENSOR, 2)
    # # print(f'sensor2: {sensor2}')

    # sensor0 = read(READ_SENSOR, 0)
    # # print(f'sensor0: {sensor0}')
    # sensor1 = read(READ_SENSOR, 1)
    # # print(f'sensor1: {sensor1}')
    # sensor2 = read(READ_SENSOR, 2)
    # # print(f'sensor2: {sensor2}')

    # sensor0 = read(READ_SENSOR, 0)
    # # print(f'sensor0: {sensor0}')
    # sensor1 = read(READ_SENSOR, 1)
    # # print(f'sensor1: {sensor1}')
    # sensor2 = read(READ_SENSOR, 2)
    # # print(f'sensor2: {sensor2}')
    # print()
    end_time = time.time()
    print(end_time-start_time)