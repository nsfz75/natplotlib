#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

struct Point {
    float x;
    float y;
};

class Plot {
public:
    Plot(int w, int h) : width(w), height(h) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            SDL_Log("SDL_Init Error: %s", SDL_GetError());
        }

        window = SDL_CreateWindow("C++ Plot",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width, height, 0);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    ~Plot() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    Plot(const Plot&) = delete;
    Plot& operator=(const Plot&) = delete;

    void addData(const std::vector<Point>& dataPoints) {
        data = dataPoints;
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (data.size() < 2) return;

        float xmin = data[0].x, xmax = data[0].x;
        float ymin = data[0].y, ymax = data[0].y;

        for (const auto& p : data) {
            xmin = std::min(xmin, p.x);
            xmax = std::max(xmax, p.x);
            ymin = std::min(ymin, p.y);
            ymax = std::max(ymax, p.y);
        }

        if (xmax == xmin || ymax == ymin) return;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        for (size_t i = 1; i < data.size(); i++) {
            int x1 = mapX(data[i - 1].x, xmin, xmax);
            int y1 = mapY(data[i - 1].y, ymin, ymax);

            int x2 = mapX(data[i].x, xmin, xmax);
            int y2 = mapY(data[i].y, ymin, ymax);

            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        SDL_RenderPresent(renderer);
    }

    bool isOpen() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
        }
        return true;
    }

private:
    int width, height;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Point> data;

    int mapX(float x, float xmin, float xmax) {
        return static_cast<int>((x - xmin) / (xmax - xmin) * width);
    }

    int mapY(float y, float ymin, float ymax) {
        return static_cast<int>(height - (y - ymin) / (ymax - ymin) * height);
    }
};