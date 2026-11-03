
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Read PiBerry's CPU temperature in Celsius
double readTemperature() {
    std::ifstream sensor("/sys/class/thermal/thermal_zone0/temp");
    int raw = 0;

    if (!sensor || !(sensor >> raw)) {
        std::cerr << "Cannot read CPU temperature\n";
        return -1.0;
    }

    return raw / 1000.0;
}

int main() {
    // Prevent disconnected clients from terminating the server
    std::signal(SIGPIPE, SIG_IGN);

    // Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Allow the port to be reused after restarting
    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
               &reuse, sizeof(reuse));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind the server to port 8080
    if (bind(serverSocket,
             reinterpret_cast<sockaddr*>(&address),
             sizeof(address)) < 0) {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    std::cout << "PiBerry telemetry server listening on port 8080\n";

    while (true) {
        // Wait for a browser to connect
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket < 0) {
            continue;
        }

        // Read the HTTP request
        char buffer[2048] = {};
        ssize_t bytesRead = recv(
            clientSocket, buffer, sizeof(buffer) - 1, 0
        );

        if (bytesRead <= 0) {
            close(clientSocket);
            continue;
        }

        // Only respond to GET /temperature
        std::string request(buffer, bytesRead);

        if (request.rfind("GET /temperature ", 0) != 0) {
            const std::string response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n\r\n";

            send(clientSocket, response.c_str(),
                 response.size(), MSG_NOSIGNAL);
            close(clientSocket);
            continue;
        }

        double temperature = readTemperature();

        std::string body;

        if (temperature < 0) {
            body = "{\"error\":\"Sensor unavailable\"}";
        } else {
            body = "{\"temperature\":" +
                   std::to_string(temperature) + "}";
        }

        // Build the HTTP response
        std::string response =
            std::string("HTTP/1.1 ") +
            (temperature < 0
                ? "500 Internal Server Error\r\n"
                : "200 OK\r\n") +
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Cache-Control: no-store\r\n"
            "Connection: close\r\n"
            "Content-Length: " +
            std::to_string(body.size()) +
            "\r\n\r\n" + body;

        // Send the response to the browser
        send(clientSocket, response.c_str(),
             response.size(), MSG_NOSIGNAL);

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
