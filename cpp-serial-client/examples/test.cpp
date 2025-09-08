#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>
#include <chrono>
#include <thread>
#include <cstring>  // memcpy
#include <libserial/SerialPort.h>

using namespace LibSerial;

// ------------------- Protocol Constants -------------------
const uint8_t START_BYTE = 0xAA;
const uint8_t READ_SENSOR = 0x01;
const uint8_t CMD_LED     = 0x02;
const uint8_t CMD_MOTOR   = 0x03;

// ------------------- Globals -------------------
SerialPort serial_port;

// ------------------- Helpers -------------------
uint8_t calcChecksum(const std::vector<uint8_t>& packet) {
    uint32_t sum = 0;
    for (auto b : packet) sum += b;
    return sum & 0xFF;
}

void send_packet(uint8_t cmd) {
    std::vector<uint8_t> packet = {START_BYTE, cmd, 0}; // no payload
    uint8_t checksum = calcChecksum(packet);
    packet.push_back(checksum);
    serial_port.Write(packet);
}

void send_packet_stream(uint8_t cmd, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> packet = {START_BYTE, cmd, (uint8_t)payload.size()};
    packet.insert(packet.end(), payload.begin(), payload.end());
    uint8_t checksum = calcChecksum(packet);
    packet.push_back(checksum);
    serial_port.Write(packet);
}

float read_packet() {
    std::vector<uint8_t> payload(4);
    serial_port.Read(payload, 4);
    float val;
    std::memcpy(&val, payload.data(), sizeof(float)); // little-endian assumed
    return val;
}

std::tuple<float, float, float, float> read_packet_stream() {
    std::vector<uint8_t> payload(16);
    serial_port.Read(payload, 16);

    float vals[4];
    std::memcpy(&vals[0], payload.data() + 0, 4);
    std::memcpy(&vals[1], payload.data() + 4, 4);
    std::memcpy(&vals[2], payload.data() + 8, 4);
    std::memcpy(&vals[3], payload.data() + 12, 4);

    return {vals[0], vals[1], vals[2], vals[3]};
}

// ------------------- High-Level Wrappers -------------------
float write_data(uint8_t cmd, uint8_t pos, float val) {
    std::vector<uint8_t> payload(sizeof(uint8_t) + sizeof(float));
    payload[0] = pos;
    std::memcpy(&payload[1], &val, sizeof(float));
    send_packet_stream(cmd, payload);
    return read_packet();
}

float read_data(uint8_t cmd) {
    float zero = 0.0f;
    std::vector<uint8_t> payload(sizeof(float));
    std::memcpy(payload.data(), &zero, sizeof(float));
    send_packet_stream(cmd, payload);
    return read_packet();
}

float write_data_stream(uint8_t cmd, float a, float b, float c, float d) {
    std::vector<uint8_t> payload(4 * sizeof(float));
    std::memcpy(&payload[0],  &a, 4);
    std::memcpy(&payload[4],  &b, 4);
    std::memcpy(&payload[8],  &c, 4);
    std::memcpy(&payload[12], &d, 4);
    send_packet_stream(cmd, payload);
    return read_packet();
}

std::tuple<float, float, float, float> read_data_stream(uint8_t cmd) {
    send_packet(cmd);
    return read_packet_stream();
}

// ------------------- Main -------------------
int main() {
    try {
        serial_port.Open("/dev/ttyUSB0");
        serial_port.SetBaudRate(BaudRate::BAUD_921600);
        serial_port.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
        serial_port.SetStopBits(StopBits::STOP_BITS_1);
        serial_port.SetParity(Parity::PARITY_NONE);
        serial_port.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);
    } catch (const OpenFailed&) {
        std::cerr << "Failed to open serial port!" << std::endl;
        return -1;
    }

    uint8_t state = 0;

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        state = !state;
        write_data(CMD_LED, 255, state);

        float w = 1.689f;
        float x = 3.678f;
        float y = 9.3456f;
        float z = 45.90f;

        // send motor data
        write_data_stream(CMD_MOTOR, w, x, y, z);
        // read sensor data
        auto [a, b, c, d] = read_data_stream(READ_SENSOR);
        auto [ax, bx, cx, dx] = read_data_stream(READ_SENSOR);

        auto [ay, by, cy, dy] = read_data_stream(READ_SENSOR);
        auto [az, bz, cz, dz] = read_data_stream(READ_SENSOR);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Loop time: " << elapsed.count() << "s"<< std::endl;
        
        // std::cout << "Loop time: " << elapsed.count() << "s | "
        //           << "Sensor: " << a << ", " << b << ", " << c << ", " << d
        //           << std::endl;

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    serial_port.Close();
    return 0;
}
