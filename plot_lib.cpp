#include <iostream>
#include <functional>
#include "plot_lib.hpp"

void plot_function(std::function<float(float)> func, int min_x, int max_x) {
    Plot plot(1200, 900);

    std::vector<Point> data;

    for (float x = min_x; x < max_x; x += 0.1f) {
        float y = func(x);
        data.push_back({x, y});
    }

    plot.addData(data);

    while (plot.isOpen()) {
        plot.render();
        SDL_Delay(16);
    }
}

void WinMain() {
    plot_function([](float x) {return (x + 5);}, -200, 200);
}
