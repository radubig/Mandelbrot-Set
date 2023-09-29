# Mandelbrot-Set

A cross-compatible program that renders the Mandelbrot Set in real-time using 1D textures as the source of the fractal gradient. Supports scrolling, continuous zooming, animations and different interactions using the mouse and keyboard, and offers a GUI that allows the user to customize parameters.

The project uses the OpenGL as the graphics API, [Dear ImGui](https://github.com/ocornut/imgui) for the GUI and is implemented with [GLFW](https://www.glfw.org/) and [GLEW](https://glew.sourceforge.net/).
<!-- TODO: make a dependencies tab -->

Currently the program has been tested on Windows and Linux.

## Controls

Besides the GUI, the user can interact with the fractal using the following controls:

|        Key        | Control                       |
|:-----------------:|-------------------------------|
|  `Num+` / `Num-`  | Zoom in / out                 |
|      Scroll       | Zoom in / out                 |
|        `Z`        | Auto Zoom                     |
| Scroll + Hold `S` | Change *max_iterations*       |
|        `R`        | Reset viewport and parameters |
|        `N`        | Change fractal texture        |
|        `F`        | Trigger animation (frequency) |
|        `G`        | Trigger animation (UV-coord)  |

## Compiling

This repo uses *Dear ImGui* as a submodule. In order to clone all required files for building, use
```git
git clone --recursive --depth 1 https://github.com/radubig/Mandelbrot-Set 
```


The project uses CMake to generate project files or makefiles for your chosen development environment.

First, use CMake to generate the configuration files. As an example, we'll generate the files in a folder called `build`:
```bash
mkdir build
cd build
cmake ..
```
*Note: You may need to explicitly specify a generator for cmake if it fails to find the corresponding one for your system. On windows, if you have MinGW installed, you may want to run:*
```bash
cmake --fresh -G "MinGW Makefiles" ..
```
After generating the configuration files, build the program using
```bash
cmake --build . --target MandelbrotSet --config Release
```
and install it in a folder of your choice (in this example, it will be installed in `Release`):
```bash
cmake --install . --prefix ../Release
```
