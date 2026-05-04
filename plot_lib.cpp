#include <iostream>
#include <functional>
#include "plot_lib.hpp"

struct Limits {
    float min;
    float max;
};

struct Colour {
    unsigned char r, g, b, a;
};

template <typename Func>


void plot_function(Func func, Limits x_lim, Limits y_lim, Colour plot_colour, Colour background_colour) {
    Plot plot(1200, 900);

    std::vector<Point> data;

    for (float x = x_lim.min; x < x_lim.max; x += 0.1f) {
        float y = func(x);
        data.push_back({x, y});
    }

    plot.addData(data);
    plot.setLimits(x_lim.min, x_lim.max, y_lim.min, y_lim.max);
    plot.setPlotColour(plot_colour.r, plot_colour.g, plot_colour.b, plot_colour.a);
    //plot.setBackgroundColour(background_colour.r, background_colour.g, background_colour.b, background_colour.a);

    while (plot.isOpen()) {
        plot.render();
        SDL_Delay(16);
    }
}

void WinMain() {
    plot_function([](float x) {return (sin(x));},
    {0, 10},
    {-1, 1},
    {255, 0, 0, 255},
    {255, 0, 0, 255}
    );
}
