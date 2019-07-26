/*
Title: Rendering to Textures
File Name: fragment.glsl
Copyright ? 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#version 400 core

in vec2 uv;
in vec4 color;

// sampler2D is the type we use in glsl to read texture data.
// It contains the texture data, and uses the parameters set in c++ to read from it.
uniform sampler2D tex;

void main(void)
{
	// texture is a glsl function that takes a sampler and a texture coordinate and returns a color.
	// We are multiplying by the vertex color, but we set the vertex color to white, so nothing happens.
	gl_FragColor = texture(tex, uv) * color;
}