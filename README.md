# Basic OpenGL with GLFW: Rendering to Textures

This program introduces basic concepts involved in writing an OpenGL program.

# About

Instead of rendering directly to the backbuffer, it's possible to render to a frame buffer texture. This opens up tons of possibilities for post-process effects, and other advanced rendering techniques. This example demonstrates the basics of setting up a frame buffer, rendering to it, and then using that data to render something else.

# Setup

You will need to have CMake installed on your computer, and properly added to your path.
In order to setup, run the following in a shell, then open the project in your preferred editor.
Windows setup has been configured for use with Visual Studio.

Windows:
```
cd path/to/folder
setup.cmd
```
Linux:
```
cd path/to/folder
./setup
```
