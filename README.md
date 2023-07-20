# Mandelbrot-Set

A cross-compatible program that renders the Mandelbrot Set in real-time using 1D textures as the source of the fractal gradient. Supports scrolling, continuous zooming, animations and different interactions using the mouse and keyboard.

The project uses the OpenGL API and is implemented with [GLFW](https://www.glfw.org/) and [GLEW](https://glew.sourceforge.net/).

Currently the program has been tested on Windows and Linux.

## Controls

|        Key        | Control                       |
|:-----------------:|-------------------------------|
|  `Num+` / `Num-`  | Zoom in / out                 |
|      Scroll       | Zoom in / out                 |
|        `Z`        | Auto Zoom                     |
| Scroll + Hold `S` | Change *max_iterations*       |
|        `R`        | Reset viewport                |
|        `N`        | Change fractal texture        |
|        `F`        | Trigger animation (frequency) |
|        `G`        | Trigger animation (UV-coord)  |
|        `A`        | Show FPS                      |
|        `P`        | Print Stats                   |
|        `L`        | Load custom settings          |

## Build

To build the program, use CMake to generate the configuration files required for installing the program.
