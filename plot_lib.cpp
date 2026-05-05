#include <iostream>
#include <functional>
#include "plot_lib.hpp"

template <typename Func>

void plot_function(Func func, plot::Bounds limits, format::ColourStyle plot_colour, int plot_thickness, format::ColourStyle background_colour, plot::Step step_size, format::ColourStyle grid_colour, ui::plotTitle title_config, ui::plotTitle x_label_config, ui::plotTitle y_label_config) {
    Plot plot(1200, 900);

    std::vector<plot::Point> data;

    for (float x = limits.min_x; x < limits.max_x; x += 0.1f) {
        float y = func(x);
        data.push_back({x, y});
    }

    plot.addData(data);
    plot.set_title(title_config);
    plot.set_x_label(x_label_config);
    plot.set_y_label(y_label_config);
    plot.set_limits(limits);
    plot.set_plot_colour(plot_colour, plot_thickness);
    plot.set_background_colour(background_colour);
    plot.set_grid_state(grid_colour, step_size);

    while (plot.isOpen()) {
        plot.render();
        SDL_Delay(16);
    }
}

void WinMain() {
    plot_function([](float x) {return (sin(x)/x);},         // function
    {-20, 20, -0.5, 1.5, true},                             // {x min, x max, y min, y max, custom limits enabled}
    {{255, 0, 0, 255}, true}, {2},                          // {{red colour value, green colour value, blue colour value}, custom colour enabled} , {line thickness}
    {{255, 0, 0, 255}, false},                              // {{red colour value, green colour value, blue colour value}, custom colour enabled}
    {20, 20, 2, 5, true},                                     // {grid x step size, grid y step size, x-line thickness, y-line thickness, feature enabled}
    {{12, 0, 0, 255}, false},                               // {{red colour value, green colour value, blue colour value}, custom colour enabled}
    {"My test title", "arial", 36, 50, 0, true},            // {plot title, font name, font size, x position, y position, title enabled}
    {"My x axis label", "Calibri", 12, 100, 100, true},     // {x label, font name, font size, x position, y position, x label enabled}
    {"My y axis label", "Gadugi", 12, 0, 0, true}           // {y label, font name, font size, x position, y position, y label enabled}
    );
}
