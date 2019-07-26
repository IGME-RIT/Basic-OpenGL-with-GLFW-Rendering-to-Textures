/*
Title: Rendering to Textures
File Name: invertFrag.glsl
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
uniform sampler2D tex;

void main(void)
{
	vec2 coord;

	// draw the original image from the 1st render
	// in the lower corner of the screen, 20% of the window
	if (uv.x < 0.2 && uv.y < 0.2)
	{
		// multiply by 5, so 100% of the original image
		// from the 1st render is in 20% of the final image
		coord = vec2(uv.x * 5, uv.y * 5);
	}

	else
	{
		// We can achieve a bending effect by taking the sin of our x coordinate and adding it to the y coordinate
		// (they have to be scaled a little bit to account for the range of numbers)
		coord = vec2(uv.x, uv.y + sin(uv.x * 10) * .1);
	}

	// Once we have that coordinate, we can read from the texture at that location and output it to the screen.
	gl_FragColor = texture(tex, coord);
}