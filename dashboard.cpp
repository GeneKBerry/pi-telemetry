
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

double readTemperature() {
    std::ifstream sensor("/sys/class/thermal/thermal_zone0/temp");
    int raw = 0;

    if (!sensor || !(sensor >> raw)) {
        std::cerr << "Cannot read CPU temperature\n";
        return -1;
    }

    return raw / 1000.0;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "PiBerry CPU Temperature",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 480,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<double> temperatures;
    bool running = true;
    SDL_Event event;
    Uint32 lastUpdate = SDL_GetTicks() - 1000;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        Uint32 now = SDL_GetTicks();

        if (now - lastUpdate >= 1000) {
            double temp = readTemperature();

            if (temp >= 0) {
                temperatures.push_back(temp);

                if (temperatures.size() > 100) {
                    temperatures.erase(temperatures.begin());
                }

                std::cout << "CPU: " << temp << " C" << std::endl;
            }

            lastUpdate = now;
        }

        // Dark background
        SDL_SetRenderDrawColor(renderer, 15, 23, 42, 255);
        SDL_RenderClear(renderer);

        // Graph grid
        SDL_SetRenderDrawColor(renderer, 55, 65, 81, 255);

        for (int y = 60; y <= 420; y += 60) {
            SDL_RenderDrawLine(renderer, 50, y, 750, y);
        }

        // Temperature line: 0 to 100 Celsius
        SDL_SetRenderDrawColor(renderer, 56, 189, 248, 255);

        for (size_t i = 1; i < temperatures.size(); i++) {
            int x1 = 50 + static_cast<int>((i - 1) * 7);
            int x2 = 50 + static_cast<int>(i * 7);

            int y1 = 420 - static_cast<int>((temperatures[i - 1] - 30.0) * 12.0);
            int y2 = 420 - static_cast<int>((temperatures[i] - 30.0) * 12.0);

            y1 = std::clamp(y1, 60, 420);
            y2 = std::clamp(y2, 60, 420);

            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
