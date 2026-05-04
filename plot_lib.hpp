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
        if (use_custom_background)
            SDL_SetRenderDrawColor(renderer, rvalue_background, gvalue_background, bvalue_background, avalue_background);
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderClear(renderer);

        if (data.size() < 2) return;

        float xmin_local, xmax_local, ymin_local, ymax_local;

        if (use_custom_limits) {
            xmin_local = xmin;
            xmax_local = xmax;
            ymin_local = ymin;
            ymax_local = ymax;
        } else {
            xmin_local = data[0].x;
            xmax_local = data[0].x;
            ymin_local = data[0].y;
            ymax_local = data[0].y;

            for (const auto& p : data) {
                xmin_local = std::min(xmin_local, p.x);
                xmax_local = std::max(xmax_local, p.x);
                ymin_local = std::min(ymin_local, p.y);
                ymax_local = std::max(ymax_local, p.y);
            }
        }

        if (xmax_local == xmin_local || ymax_local == ymin_local) return;

        if (use_custom_colours)
            SDL_SetRenderDrawColor(renderer, rvalue, gvalue, bvalue, avalue);
        else 
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        for (size_t i = 1; i < data.size(); i++) {
            int x1 = mapX(data[i - 1].x, xmin_local, xmax_local);
            int y1 = mapY(data[i - 1].y, ymin_local, ymax_local);

            int x2 = mapX(data[i].x, xmin_local, xmax_local);
            int y2 = mapY(data[i].y, ymin_local, ymax_local);
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

    void setLimits(float x_min, float x_max, float y_min, float y_max) {
        xmin = x_min;
        xmax = x_max;
        ymin = y_min;
        ymax = y_max;
        use_custom_limits = true;
    }

    void setPlotColour(int r_value, int g_value, int b_value, int a_value) {
        rvalue = r_value;
        gvalue = g_value;
        bvalue = b_value;
        avalue = a_value;
        use_custom_colours = true;
    }

    void setBackgroundColour(int r_value, int g_value, int b_value, int a_value) {
        rvalue_background = r_value;
        gvalue_background = g_value;
        bvalue_background = b_value;
        avalue_background = a_value;
        use_custom_background = true;
    }

private:
    int width, height;
    int rvalue, gvalue, bvalue, avalue;
    int rvalue_background, gvalue_background, bvalue_background, avalue_background;
    float xmin, xmax, ymin, ymax;
    bool use_custom_limits = false;
    bool use_custom_colours = false;
    bool use_custom_background = false;
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