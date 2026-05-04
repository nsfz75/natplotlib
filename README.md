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
* Axis rendering (x = 0, y = 0)
* Title rendering using TrueType fonts
* Real-time rendering loop

---

## Example

```cpp
plot_function(
    [](float x) { return sin(x); }, // function
    {0, 10},                        // x-lims
    {-1, 1},                        // y-lims
    {255, 0, 0, 255, true},         // plot colour
    {255, 255, 255, 255, false},    // background colour
    {50, 50, true},                 // grid
    {200, 200, 200, 255, true}      // grid colour
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

## Fonts

Fonts are loaded from:

```
C:/Windows/Fonts/
```

Example:

```cpp
plot.setTitle("My Graph", "arial", 24);
```

---

## Future Work

* Axis labels
* Improved text alignment
* Multiple plot support
* Line thickness control
* Zoom and pan
* Image export
