#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>

int main() {

    while (true) {
        
    int temperature;

    std::ifstream sensor("/sys/class/thermal/thermal_zone0/temp");

    if (!sensor.is_open()) {
        std::cerr << "Error: Cannot read CPU temperature." << std::endl;
        return 1;
    }

    sensor >> temperature;
    sensor.close();

    double celsius = temperature / 1000.0;

    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::cout << "Raspberry Pi Telemetry Station" << std::endl;
    std::cout << "["
    << localTime->tm_hour << ":"
    << localTime->tm_min << ":"
    << localTime->tm_sec << "] "
    << "CPU Temperature: "
    << celsius << " C"
    << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}