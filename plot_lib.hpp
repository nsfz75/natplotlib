#pragma once

#include <SDL2/SDL_ttf.h>
#include <vector>
#include <algorithm>

struct Limits {
    float min, max;
};

struct Step {
    float x_step, y_step;
    bool enabled;
};

struct Colour {
    unsigned char r, g, b, a;
};

struct ColourStyle {
    Colour colour;
    bool enabled;
};

struct plotTitle {
    std::string title;
    std::string font_name;
    int fontsize;
    float x_position;
    float y_position;
    bool state;
};

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
        clearScreen();

        if (data.size() < 2) return;

        auto [xmin_local, xmax_local, ymin_local, ymax_local] = computeLimits();

        if (xmax_local == xmin_local || ymax_local == ymin_local) return;

        draw_grid(xmin_local, xmax_local, ymin_local, ymax_local);
        draw_axes(xmin_local, xmax_local, ymin_local, ymax_local);
        draw_plot(xmin_local, xmax_local, ymin_local, ymax_local);
        draw_title();

        SDL_RenderPresent(renderer);
    }

    void clearScreen() {
        if (background_colour.enabled)
            SDL_SetRenderDrawColor(renderer, background_colour.colour.r, background_colour.colour.g, background_colour.colour.b, background_colour.colour.a);
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderClear(renderer);
    }

    std::tuple<float,float,float,float> computeLimits() {
        float xmin_local, xmax_local, ymin_local, ymax_local;

        if (use_custom_limits) {
            xmin_local = x_limits.min;
            xmax_local = x_limits.max;
            ymin_local = y_limits.min;
            ymax_local = y_limits.max;
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

        return {xmin_local, xmax_local, ymin_local, ymax_local};
    }

    void draw_grid(float xmin_local, float xmax_local, float ymin_local, float ymax_local) {
        if (!use_grid) return;

        if (grid_colour.enabled)
            generate_grid(xmin_local, xmax_local, ymin_local, ymax_local, step_grid.x_step, step_grid.y_step, grid_colour.colour.r, grid_colour.colour.g, grid_colour.colour.b, grid_colour.colour.a);
        else
            generate_grid(xmin_local, xmax_local, ymin_local, ymax_local, step_grid.x_step, step_grid.y_step, 220, 220, 220, 255);
    }

    void draw_axes(float xmin_local, float xmax_local, float ymin_local, float ymax_local) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

        if (0 >= xmin_local && 0 <= xmax_local) {
            int sx = mapX(0, xmin_local, xmax_local);
            SDL_RenderDrawLine(renderer, sx, 0, sx, height);
        }

        if (0 >= ymin_local && 0 <= ymax_local) {
            int sy = mapY(0, ymin_local, ymax_local);
            SDL_RenderDrawLine(renderer, 0, sy, width, sy);
        }
    }

    void draw_plot(float xmin_local, float xmax_local, float ymin_local, float ymax_local) {
        if (line_colour.enabled)
            SDL_SetRenderDrawColor(renderer, line_colour.colour.r, line_colour.colour.g, line_colour.colour.b, line_colour.colour.a);
        else 
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        for (size_t i = 1; i < data.size(); i++) {
            int x1 = mapX(data[i - 1].x, xmin_local, xmax_local);
            int y1 = mapY(data[i - 1].y, ymin_local, ymax_local);

            int x2 = mapX(data[i].x, xmin_local, xmax_local);
            int y2 = mapY(data[i].y, ymin_local, ymax_local);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }

    void draw_title() {
        if (add_title)
            generate_title(plot_title, title_location.x, title_location.y);
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

    void generate_grid(float xmin, float xmax, float ymin, float ymax, float x_step_grid, float y_step_grid, int rvalue, int gvalue, int bvalue, int avalue) {
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

    void generate_title(const std::string& text, int x, int y) {
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

    void setLimits(const Limits& x_lim, const Limits& y_lim) {
        x_limits.min = x_lim.min;
        x_limits.max = x_lim.max;
        y_limits.min = y_lim.min;
        y_limits.max = y_lim.max;
        use_custom_limits = true;
    }

    void setPlotColour(const ColourStyle& c) {
        line_colour.colour.r = c.colour.r;
        line_colour.colour.g = c.colour.g;
        line_colour.colour.b = c.colour.b;
        line_colour.colour.a = c.colour.a;
        line_colour.enabled = c.enabled;
    }

    void setBackgroundColour(const ColourStyle& c) {
        background_colour.colour.r = c.colour.r;
        background_colour.colour.g = c.colour.g;
        background_colour.colour.b = c.colour.b;
        background_colour.colour.a = c.colour.a;
        background_colour.enabled = c.enabled;
    }

    void setGridState(const ColourStyle& c, const Step& step_size) {
        grid_colour.colour.r = c.colour.r;
        grid_colour.colour.g = c.colour.b;
        grid_colour.colour.b = c.colour.b;
        grid_colour.colour.a = c.colour.a;
        grid_colour.enabled = c.enabled;
        step_grid.x_step = step_size.x_step;
        step_grid.y_step = step_size.y_step;
        use_grid = step_size.enabled;
    }

    void setTitle(const std::string& title, const std::string& font_name, int fontsize, float x_position, float y_position, bool state) {
        plot_title = title;
        plot_font = font_name;
        plot_fontsize = fontsize;
        title_location.x = x_position;
        title_location.y = y_position;
        add_title = state;
        loadFont();
    }

private:
    int width, height;
    ColourStyle line_colour;
    ColourStyle background_colour;
    ColourStyle grid_colour;
    Limits x_limits;
    Limits y_limits;
    Step step_grid;
    Point title_location;
    bool use_custom_limits = false;
    bool use_grid = false;
    bool add_title = false;

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