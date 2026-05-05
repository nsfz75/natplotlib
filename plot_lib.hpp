#pragma once

#include <SDL2/SDL_ttf.h>
#include <vector>
#include <algorithm>

namespace plot {
    // Plot bounds
    struct Bounds {
        float min_x, max_x;
        float min_y, max_y;
        bool enabled;
    };

    // Grid config
    struct Step {
        float x_step, y_step;
        int x_line_thickness, y_line_thickness;
        bool enabled;
    };

    // Single point
    struct Point {
        float x;
        float y;
    };
}

namespace format {
    // Colour values
    struct Colour {
        unsigned char r, g, b, a;
    };

    // Master colour config
    struct ColourStyle {
        Colour colour;
        bool enabled;
    };
}

namespace ui {
    // Text config
    struct plotTitle {
        std::string title;
        std::string font_name;
        int fontsize;
        float x_position;
        float y_position;
        bool enabled;
    };
}

class Plot {
public:
    Plot(int w, int h) : width(w), height(h) {
        // Initialise SDL video subsystem
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            SDL_Log("SDL_Init Error: %s", SDL_GetError());
        }

        // Initialise font subsystem
        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init Error: %s", TTF_GetError());
        }

        // Create rendering window
        window = SDL_CreateWindow("C++ Plot",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width, height, 0);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    ~Plot() {
        // Clean up SDL resources
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        // Release loaded fonts
        TTF_CloseFont(title_font);
        TTF_CloseFont(x_label_font);
        TTF_CloseFont(y_label_font);
        TTF_Quit();
        SDL_Quit();
    }

    Plot(const Plot&) = delete;
    Plot& operator=(const Plot&) = delete;

    void addData(const std::vector<plot::Point>& dataPoints) {
        data = dataPoints;
    }

    void render() {
        clearScreen();

        if (data.size() < 2) return;

        // Determine plotting bounds
        auto [xmin_local, xmax_local, ymin_local, ymax_local] = compute_limits();

        // Avoid division by zero in mapping
        if (xmax_local == xmin_local || ymax_local == ymin_local) return;

         // Draw components
        draw_grid(xmin_local, xmax_local, ymin_local, ymax_local);
        draw_axes(xmin_local, xmax_local, ymin_local, ymax_local);
        draw_plot(xmin_local, xmax_local, ymin_local, ymax_local);

        // UI overlays
        draw_title();
        draw_x_label();
        draw_y_label();

        SDL_RenderPresent(renderer);
    }

    void clearScreen() {
        if (background_colour.enabled)
            SDL_SetRenderDrawColor(renderer, background_colour.colour.r, background_colour.colour.g, background_colour.colour.b, background_colour.colour.a);
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderClear(renderer);
    }

    std::tuple<float,float,float,float> compute_limits() {
        float xmin_local, xmax_local, ymin_local, ymax_local;

        if (limits.enabled) {
            xmin_local = limits.min_x;
            xmax_local = limits.max_x;
            ymin_local = limits.min_y;
            ymax_local = limits.max_y;
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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

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

            for (int i = -plot_thickness/2; i <= plot_thickness/2; i++) {
                SDL_RenderDrawLine(renderer, x1 + i, y1, x2 + i, y2); // horizontal offset
                SDL_RenderDrawLine(renderer, x1, y1 + i, x2, y2 + i); // vertical offset
            }
        }
    }

    void draw_title() {
        if (plot_title_config.enabled)
            generate_text(title_font, plot_title_config.title, plot_title_config.x_position, plot_title_config.y_position);
    }

    void draw_x_label() {
        if (x_label_config.enabled)
            generate_text(x_label_font, x_label_config.title, x_label_config.x_position, x_label_config.y_position);
    }

    void draw_y_label() {
        if (y_label_config.enabled)
            generate_text(y_label_font, y_label_config.title, y_label_config.x_position, y_label_config.y_position);
    }

    TTF_Font* loadFont(const ui::plotTitle& cfg)
    {
        std::string path = "C:/Windows/Fonts/" + cfg.font_name + ".ttf";
        TTF_Font* f = TTF_OpenFont(path.c_str(), cfg.fontsize);

        if (!f)
            SDL_Log("Font load error: %s", TTF_GetError());

        return f;
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
            for (int i = -step_grid.y_line_thickness/2; i <= step_grid.y_line_thickness/2; i++)
                SDL_RenderDrawLine(renderer, sx + i, 0, sx + i, height);
        }

        // horizontal lines
        for (float y = ymin; y <= ymax; y += y_step)
        {
            int sy = mapY(y, ymin, ymax);
            for (int i = -step_grid.x_line_thickness/2; i <= step_grid.x_line_thickness/2; i++)
                SDL_RenderDrawLine(renderer, 0, sy + i, width, sy + i);
        }
    }

    void generate_text(TTF_Font* font, const std::string& text, int x, int y) {
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

    void set_limits(const plot::Bounds& plot_limits) {
        limits.min_x = plot_limits.min_x;
        limits.max_x = plot_limits.max_x;
        limits.min_y = plot_limits.min_y;
        limits.max_y = plot_limits.max_y;
        limits.enabled = plot_limits.enabled;
    }

    void set_plot_colour(const format::ColourStyle& c, int line_thickness) {
        line_colour.colour.r = c.colour.r;
        line_colour.colour.g = c.colour.g;
        line_colour.colour.b = c.colour.b;
        line_colour.colour.a = c.colour.a;
        line_colour.enabled = c.enabled;
        plot_thickness = line_thickness;
    }

    void set_background_colour(const format::ColourStyle& c) {
        background_colour.colour.r = c.colour.r;
        background_colour.colour.g = c.colour.g;
        background_colour.colour.b = c.colour.b;
        background_colour.colour.a = c.colour.a;
        background_colour.enabled = c.enabled;
    }

    void set_grid_state(const format::ColourStyle& c, const plot::Step& step_size) {
        grid_colour.colour.r = c.colour.r;
        grid_colour.colour.g = c.colour.g;
        grid_colour.colour.b = c.colour.b;
        grid_colour.colour.a = c.colour.a;
        grid_colour.enabled = c.enabled;
        step_grid.x_step = step_size.x_step;
        step_grid.y_step = step_size.y_step;
        step_grid.x_line_thickness = step_size.x_line_thickness;
        step_grid.y_line_thickness = step_size.y_line_thickness;
        use_grid = step_size.enabled;
    }

    void set_title(const ui::plotTitle& title_config) {
        plot_title_config = title_config;
        update_font(plot_title_config, title_font);
    }

    void set_x_label(const ui::plotTitle& label_config) {
        x_label_config = label_config;
        update_font(x_label_config, x_label_font);
    }

    void set_y_label(const ui::plotTitle& label_config) {
        y_label_config = label_config;
        update_font(y_label_config, y_label_font);
    }

    void update_font(ui::plotTitle& config, TTF_Font*& font) {
        TTF_Font* new_font = loadFont(config);

        if (new_font) {
            if (font) TTF_CloseFont(font);
            font = new_font;
        }
    }

private:
    bool use_grid = false;
    int plot_thickness = 1;
    int width, height;

    format::ColourStyle line_colour;
    format::ColourStyle background_colour;
    format::ColourStyle grid_colour;

    plot::Bounds limits;

    plot::Step step_grid;

    ui::plotTitle plot_title_config;
    ui::plotTitle x_label_config;
    ui::plotTitle y_label_config;

    TTF_Font* title_font = nullptr;
    TTF_Font* x_label_font = nullptr;
    TTF_Font* y_label_font = nullptr;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::vector<plot::Point> data;

    int mapX(float x, float xmin, float xmax) {
        return static_cast<int>((x - xmin) / (xmax - xmin) * width);
    }

    int mapY(float y, float ymin, float ymax) {
        return static_cast<int>(height - (y - ymin) / (ymax - ymin) * height);
    }
};