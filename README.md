# OpenGL-GLFW
OpenGL-GLFW


This repository is mainly for learning modern OpenGL.

This time I chose GLFW for windowing and events instead of my usual SDL2 just for fun
and learning, in the future I will be using SDL2 with GL anyway, but this is no issue

### Build instructions

Only GLFW, GLEW and assimp needs to be installed to build this, then use make

For MSYS2 using mingw64:

```
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-assimp
```

To run:

```
./configure.sh
./build.sh
./run.sh
```

### Used Libraries

 * [CMake](https://cmake.org/) - For building
 * [OpenGL](https://www.opengl.org/) - Rendering API
 * [GLFW](https://www.glfw.org/) - Windowing and input events
 * [GLEW](https://glew.sourceforge.net/) - GL Extension loading library
 * [GLM](https://github.com/g-truc/glm) - OpenGL Mathematics Library for vector and matrix operations
 * [stb_image](https://github.com/nothings/stb) - For texture loading
 * [assimp](https://github.com/assimp/assimp) - For 3D model loading(waveform for now)
 * [ImGui](https://github.com/ocornut/imgui) - For UI

### TODO

 * Add attenuation to spot lights
 * Implement proper UV mapping for 3D Models
 * Implement sprite class, which possibly uses batch renderer
