# C++ Plotting Library (SDL2)

A lightweight 2D plotting tool written in C++ using SDL2 and SDL_ttf.
Designed for rendering mathematical functions with configurable visuals and real-time display.

---

## Features

* Function plotting via lambdas or function objects
* Custom X/Y limits
* Adjustable plot colour and background
* Grid system with configurable spacing
* Automatic or manual scaling
* Axis rendering
* Title rendering using TrueType fonts

---

## Example

```cpp
plot_function([](float x) {return (sin(x)/x);},             // function
    {-20, 20, -0.5, 1.5, true},                             // {x min, x max, y min, y max, custom limits enabled}
    {{255, 0, 0, 255}, true}, {2},                          // {{plot red colour value, plot green colour value, plot blue colour value, plot alpha value}, plot custom colour enabled} , {line thickness}
    {{255, 0, 0, 255}, false},                              // {{background red colour value, background green colour value, background blue colour value, background alpha value}, custom colour enabled}
    {20, 20, 2, 5, true},                                   // {grid x step size, grid y step size, x-line thickness, y-line thickness, grid enabled}
    {{12, 0, 0, 255}, false},                               // {{grid red colour value, grid green colour value, grid blue colour value, grid alpha balue}, custom colour enabled}
    {"My test title", "arial", 36, 50, 0, true},            // {plot title, font name, font size, x position, y position, title enabled}
    {"My x axis label", "Calibri", 12, 100, 100, true},     // {x label, font name, font size, x position, y position, x label enabled}
    {"My y axis label", "Gadugi", 12, 0, 0, true}           // {y label, font name, font size, x position, y position, y label enabled}
);
```

---

## Build

Run:

```bash
g++ plot_lib.cpp -o plot_lib.exe -lSDL2 -lSDL2_ttf -mconsole && .\plot_lib.exe
```

---

## Dependencies

* SDL2
* SDL2_ttf

---

## Future Work

* Improved text alignment
* Multiple plot support
* Line thickness control
* Zoom and pan
* Image export
