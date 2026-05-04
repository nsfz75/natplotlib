#pragma once

#include <SDL2/SDL_ttf.h>
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

        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init Error: %s", TTF_GetError());
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

        TTF_CloseFont(font);
        TTF_Quit();
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

        if (use_grid)
            drawGrid(xmin_local, xmax_local, ymin_local, ymax_local, x_step_grid, y_step_grid, 220, 220, 220, 255);
        if (use_grid && use_custom_grid_colour)
            drawGrid(xmin_local, xmax_local, ymin_local, ymax_local, x_step_grid, y_step_grid, rvalue_grid, gvalue_grid, bvalue_grid, avalue_grid);

        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

        if (0 >= xmin_local && 0 <= xmax_local) {
            int sx = mapX(0, xmin_local, xmax_local);
            SDL_RenderDrawLine(renderer, sx, 0, sx, height);
        }

        if (0 >= ymin_local && 0 <= ymax_local) {
            int sy = mapY(0, ymin_local, ymax_local);
            SDL_RenderDrawLine(renderer, 0, sy, width, sy);
        }

        if (use_custom_colours)
            SDL_SetRenderDrawColor(renderer, rvalue_line, gvalue_line, bvalue_line, avalue_line);
        else 
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        for (size_t i = 1; i < data.size(); i++) {
            int x1 = mapX(data[i - 1].x, xmin_local, xmax_local);
            int y1 = mapY(data[i - 1].y, ymin_local, ymax_local);

            int x2 = mapX(data[i].x, xmin_local, xmax_local);
            int y2 = mapY(data[i].y, ymin_local, ymax_local);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
        
        drawText(plot_title, width / 2 - 50, 10);

        SDL_RenderPresent(renderer);
    }

    void loadFont() {
        if (font) TTF_CloseFont(font);

        std::string full_path = "C:/Windows/Fonts/" + plot_font + ".ttf";
        font = TTF_OpenFont(full_path.c_str(), plot_fontsize);
        // std::cout << full_path << std::endl;

        if (!font) {
            SDL_Log("Font load error: %s", TTF_GetError());
        }
    }

    bool isOpen() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
        }
        return true;

    }

    void drawGrid(float xmin, float xmax, float ymin, float ymax, float x_step_grid, float y_step_grid, int rvalue, int gvalue, int bvalue, int avalue) {
        SDL_SetRenderDrawColor(renderer, rvalue, gvalue, bvalue, avalue);

        float x_step = (xmax - xmin) / x_step_grid;
        float y_step = (ymax - ymin) / y_step_grid;

        // vertical lines
        for (float x = xmin; x <= xmax; x += x_step)
        {
            int sx = mapX(x, xmin, xmax);
            SDL_RenderDrawLine(renderer, sx, 0, sx, height);
        }

        // horizontal lines
        for (float y = ymin; y <= ymax; y += y_step)
        {
            int sy = mapY(y, ymin, ymax);
            SDL_RenderDrawLine(renderer, 0, sy, width, sy);
        }
    }

    void drawText(const std::string& text, int x, int y) {
        SDL_Color color = {0, 0, 0, 255};

        SDL_Surface* surface =
            TTF_RenderText_Blended(font, text.c_str(), color);

        SDL_Texture* texture =
            SDL_CreateTextureFromSurface(renderer, surface);

        int w, h;
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

        SDL_Rect dst = { x, y, w, h };

        SDL_RenderCopy(renderer, texture, nullptr, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void setLimits(float x_min, float x_max, float y_min, float y_max) {
        xmin = x_min;
        xmax = x_max;
        ymin = y_min;
        ymax = y_max;
        use_custom_limits = true;
    }

    void setPlotColour(int r_value, int g_value, int b_value, int a_value, bool state) {
        rvalue_line = r_value;
        gvalue_line = g_value;
        bvalue_line = b_value;
        avalue_line = a_value;
        use_custom_colours = state;
    }

    void setBackgroundColour(int r_value, int g_value, int b_value, int a_value, bool state) {
        rvalue_background = r_value;
        gvalue_background = g_value;
        bvalue_background = b_value;
        avalue_background = a_value;
        use_custom_background = state;
    }

    void setGridState(int r_value, int g_value, int b_value, int a_value, bool state_colour, float x_step_size, float y_step_size, bool state) {
        rvalue_grid = r_value;
        gvalue_grid = g_value;
        bvalue_grid = b_value;
        avalue_grid = a_value;
        use_custom_grid_colour = state_colour;
        x_step_grid = x_step_size;
        y_step_grid = y_step_size;
        use_grid = state;
    }

    void setTitle(const std::string& title, const std::string& font_name, int fontsize) {
        plot_title = title;
        plot_font = font_name;
        plot_fontsize = fontsize;
        loadFont();
    }

private:
    int width, height;
    int rvalue_line, gvalue_line, bvalue_line, avalue_line;
    int rvalue_background, gvalue_background, bvalue_background, avalue_background;
    int rvalue_grid, gvalue_grid, bvalue_grid, avalue_grid;
    float xmin, xmax, ymin, ymax;
    float x_step_grid, y_step_grid;
    bool use_custom_limits = false;
    bool use_custom_colours = false;
    bool use_custom_background = false;
    bool use_grid = false;
    bool use_custom_grid_colour = false;
    TTF_Font* font = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Point> data;

    std::string plot_title;
    std::string plot_font;
    int plot_fontsize = 24;

    int mapX(float x, float xmin, float xmax) {
        return static_cast<int>((x - xmin) / (xmax - xmin) * width);
    }

    int mapY(float y, float ymin, float ymax) {
        return static_cast<int>(height - (y - ymin) / (ymax - ymin) * height);
    }
};