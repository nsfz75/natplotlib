#include <iostream>
#include <functional>
#include "plot_lib.hpp"

struct Limits {
    float min;
    float max;
};

struct Colour {
    unsigned char r, g, b, a;
    bool state;
};

struct minorSteps {
    float x_step, y_step;
    bool state;
};

struct plotTitle {
    const std::string& title;
    const std::string& font_name;
    int fontsize;
};

template <typename Func>


void plot_function(Func func, Limits x_lim, Limits y_lim, Colour plot_colour, Colour background_colour, minorSteps step_size, Colour grid_colour, plotTitle title_config) {
    Plot plot(1200, 900);

    std::vector<Point> data;

    for (float x = x_lim.min; x < x_lim.max; x += 0.1f) {
        float y = func(x);
        data.push_back({x, y});
    }

    plot.setTitle(title_config.title, title_config.font_name, title_config.fontsize);
    plot.addData(data);
    plot.setLimits(x_lim.min, x_lim.max, y_lim.min, y_lim.max);
    plot.setPlotColour(plot_colour.r, plot_colour.g, plot_colour.b, plot_colour.a, plot_colour.state);
    plot.setBackgroundColour(background_colour.r, background_colour.g, background_colour.b, background_colour.a, background_colour.state);
    plot.setGridState(grid_colour.r, grid_colour.g, grid_colour.b, grid_colour.a, grid_colour.state, step_size.x_step, step_size.y_step, step_size.state);

    while (plot.isOpen()) {
        plot.render();
        SDL_Delay(16);
    }
}

void WinMain() {
    plot_function([](float x) {return (sin(x));},
    {0, 10},
    {-1, 1},
    {255, 0, 0, 255, true},
    {255, 0, 0, 255, false},
    {50, 50, true},
    {12, 0, 0, 255, false},
    {"My test title", "arial", 64}
    );
}
