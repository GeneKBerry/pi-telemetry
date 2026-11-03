
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

// Read Raspberry Pi CPU temperature in Celsius
double readTemperature() {
    std::ifstream sensor("/sys/class/thermal/thermal_zone0/temp");

    int raw = 0;

    if (!sensor || !(sensor >> raw)) {
        std::cerr << "Error: Cannot read CPU temperature\n";
        return -1.0;
    }

    return raw / 1000.0;
}

int main() {
    std::cout << "PiBerry Telemetry Collector Started\n";

    while (true) {
        double temperature = readTemperature();

        if (temperature >= 0) {
            // Output data in a simple machine-readable format
            std::cout << "{\"temperature\":"
                      << temperature
                      << "}" << std::endl;
        }

        // Wait one second before collecting another reading
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    return 0;
}
