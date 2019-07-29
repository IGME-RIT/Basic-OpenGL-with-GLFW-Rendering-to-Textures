Documentation Author: Niko Procopi 2019

This tutorial was designed for Visual Studio 2017 / 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Welcome to the Render-To-Texture Tutorial!
Prerequesites: Intro Graphics

In all projects prior to this, the goal of each
sample was to create an image, and throw it on the screen.
In this tutorial, we want to intercept the image before it
gets to the screen, allowing us to save the image into a 
texture, and reuse the texture later. One render pass
is one instance of the graphics pipeline going from start
to end. Multi-pass allows us to use the graphics pipeline
several times, before our final image is done.

Multi-pass is used for shadows, lighting optimizations in 
Grand Theft Auto V, motion blur, bloom, and several other
types of post-processing. This simple tutorial has two 
passes. One of them will draw a rotating quad and save,
the image rendered from that pass to a texture. The second
pass will take the texture and warp the image, while also drawing
the original image in the lower-left corner

Each pass will require its own vertex and fragment shader.
Before we explain shaders, take a look at main.cpp

For our first pass, we need to have a GLuint to store
the texture that is rendered:
	GLuint screenTexture;
We need to have a render buffer to store our depth and
stencil information from the first pass:
	GLuint renderBuffer;
In the function resizeCallback(...), we have to change
the size of our screen texture and render buffer, every
time we change the size of our screen.

We have a function called LoadTexturePNG, which has
material that we have explained before; the method
used for creating PNG textures will be similar to 
how we store our screen image into a texture

Setting up variables for first render pass
==============================================

Skip down to line 150 of main.cpp, we make a simple square
to render to the screen in our first render pass.
Just like usual, we load a texture, we load our shaders,
we link the shaders into a shader program, and we set our uniforms
(lines 150 - 180).

Then things start changing (line 190):
In order to change the output of the fragment shader
to go to a texture instead of the screen, several
changes need to be made

We generate an empty frame buffer and then bind it:
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

We create an empty texture and bind it, this is where
we will store the final image, which has the same dimensions
as our screen, because it is basically a screenshot:
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportDimensions.x, viewportDimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

It is possible to render-to-texture without having the same dimensions as 
the screen; for example, shadow maps are usually squares and not rectangles
(more on this in a future tutorial)

We set glTexParameteri almost exactly the same as how we set PNG texture
glTexParameteri values, the concepts are the same

Setting up variables for second render pass (lines 240 - 255)
==============================================

Load teh shaders, link them to a program,
setup the uniform, super basic. This texture uniform
will read the texture that we got from the first
render pass

Drawing the first render pass (lines 270 - 300)
==============================================

We bind our framebuffer, which will redirect the
final image to our texture. We load a texture uniform,
which in this case is our PNG texture. We draw the square,
and then we unbind the texture, and we unbind the
frame buffer, which will return OpenGL to the default
frame buffer (which renders to the screen), so that
it can be used for the second pass.

Believe me, this first render pass will draw a spinning
square with a texture. You'll see this in a minute

Drawing the second render pass (lines 305 - 325)
==============================================

We activate the appropriate shader program, we give
OpenGL the texture that we rendered to (the screenshot
of the spinning square from the first render pass).
And then we draw a triangle, that's right, one triangle.

All this 2nd pass has is: 
the screenshot from the first pass, and 
a draw command to draw a triangle.

The triangle does not need a vertex buffer to draw,
this will be explained more when we explain the shaders

The Fragment shader will only activate for pixels
that appear in the rasterizer, and the rasterizer will
only detect pixels if geometry comes from the vertex shader,
so the solution is simple: draw one triangle that covers
the entire screen, to allow the fragment shader to draw
all of the pixels on the screen. Some people draw a square
that covers the screen, some people do one big triangle,
it really doesn't matter, as long as the whole screen is covered

How the shaders work:
==============================================

The first vertex shader multiplies a world matrix by
the position of each vertex, to make a square rotate.
Then, UV is passed to the rasterizer, just like previous
tutorials.

The first fragment shader sets the color of the pixel
to the color it gets from the PNG texture and a UV coordinate,
simple, just like previous tutorials.

The second vertex shader looks more complicated than it is,
the goal here is to draw a triangle without a vertex buffer.
The shader uses gl_VertexID to determine which vertex is being
processed: (vertex 0, 1, or 2). With some multiplying and
dividing, it generates the following data:

uv 0:       (0, 0)
position 0: (-1, -1)

uv 1:		(2, 0)
position 1: (3, 0)

uv 2:		(0, 2)
position 2: (0, 3)

The center of our screen is (0,0),
the rasterizer will ignore everything
outside the range of (-1, -1) to (1, 1)
positions, so therefore we are covering the
whole screen with a triangle. UV will be 
our primary use of detecting which pixel to 
handle in the 2nd fragment shader. UV is the 
UV of the screenshot from the first render pass.
UV will range from (0,0) to (1,1)

The second fragment shader draws the original
screenshot from the first fragment shader
in the corner of the screen. If UV is less than 0.2,
then it multiplies the UV by 5 (making a 0 - 1 range),
and then it sets that pixel to the pixel of the screenshot,
give that UV (from 0 - 1).

If the pixel is outside that range, we will set the UV
to something crazy:
	coord = vec2(uv.x, uv.y + sin(uv.x * 10) * .1);
	
There is no particular reason as to why the coord has
to be set to this, we only do this to warp the texture 
as an effect.

That's everything!

If you want the final image to be identical to the first image,
try this line in the 2nd fragment shader:
	gl_FragColor = texture(tex, uv);
	

How to Improve:	
Use this tutorial to make Shadows ("More Graphics" section)
Use this tutorial to optimize lighting (see Deferred Rendering "Multi-pass" section)
