#include <iostream>
#include <fstream>

int main() {
    int temperature;

    std::ifstream sensor("/sys/class/thermal/thermal_zone0/temp");

    if (!sensor.is_open()) {
        std::cerr << "Error: Cannot read CPU temperature." << std::endl;
        return 1;
    }

    sensor >> temperature;
    sensor.close();

    double celsius = temperature / 1000.0;

    std::cout << "Raspberry Pi Telemetry Station" << std::endl;
    std::cout << "CPU Temperature: " << celsius << " C" << std::endl;

    return 0;
}