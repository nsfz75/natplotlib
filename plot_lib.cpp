#include <iostream>
#include <functional>
#include "plot_lib.hpp"

template <typename Func>

void plot_function(Func func, Limits x_lim, Limits y_lim, ColourStyle plot_colour, ColourStyle background_colour, Step step_size, ColourStyle grid_colour, plotTitle title_config) {
    Plot plot(1200, 900);

    std::vector<Point> data;

    for (float x = x_lim.min; x < x_lim.max; x += 0.1f) {
        float y = func(x);
        data.push_back({x, y});
    }

    plot.setTitle(title_config.title, title_config.font_name, title_config.fontsize, title_config.x_position, title_config.y_position, title_config.state);
    plot.addData(data);
    plot.setLimits(x_lim, y_lim);
    plot.setPlotColour(plot_colour);
    plot.setBackgroundColour(background_colour);
    plot.setGridState(grid_colour, step_size);

    while (plot.isOpen()) {
        plot.render();
        SDL_Delay(16);
    }
}

void WinMain() {
    plot_function([](float x) {return (sin(x));},   // function
    {-0.3, 10},                                     // {x min, x max}
    {-1.2, 1.2},                                    // {y min, y max}
    {{255, 0, 0, 255}, true},                       // {{red colour value, green colour value, blue colour value}, custom colour enabled}
    {{255, 0, 0, 255}, false},                      // {{red colour value, green colour value, blue colour value}, custom colour enabled}
    {50, 50, true},                                 // {grid x step size, grid y step size, feature enabled}
    {{12, 0, 0, 255}, false},                       // {{red colour value, green colour value, blue colour value}, custom colour enabled}
    {"My test title", "arial", 64, 50, 0, true}     // {plot title, font name, font size, x position, y position, title enabled}
    );
}
