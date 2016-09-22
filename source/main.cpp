/*
Title: Rendering to Textures
File Name: main.cpp
Copyright � 2016
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



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "FreeImage.h"
#include <vector>
#include "../header/shape.h"
#include "../header/transform2d.h"
#include "../header/shader.h"
#include <iostream>

Shape* square;

// The transform being used to draw our shape
Transform2D transform;


glm::vec2 viewportDimensions;

// The texture we will be rendering to. It will match the dimensions of the screen.
GLuint screenTexture;

// The render buffer used to store depth and stencil information.
GLuint renderBuffer;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    viewportDimensions = glm::vec2(width, height);
	glViewport(0, 0, width, height);

    // Resize the fullscreen texture.
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportDimensions.x, viewportDimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    //
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportDimensions.x, viewportDimensions.y);
}

int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

    viewportDimensions = glm::vec2(800, 600);

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "Wat", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	// Initializes the glew library
	glewInit();


	// Indices for square (-1, -1)[2] to (1, 1)[1]
	// [0]------[1]
	//	|		 |
	//	|		 |
	//	|		 |
	// [2]------[3]

	// Create square vertex data.
	std::vector<Vertex2dUVColor> vertices;
	vertices.push_back(Vertex2dUVColor(glm::vec2(-1, 1), glm::vec2(0, 1), glm::vec4(1, 1, 1, 1)));
	vertices.push_back(Vertex2dUVColor(glm::vec2(1, 1), glm::vec2(1, 1), glm::vec4(1, 1, 1, 1)));
	vertices.push_back(Vertex2dUVColor(glm::vec2(-1, -1), glm::vec2(0, 0), glm::vec4(1, 1, 1, 1)));
    vertices.push_back(Vertex2dUVColor(glm::vec2(1, -1), glm::vec2(1, 0), glm::vec4(1, 1, 1, 1)));

	// Matching index buffer.
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);


	// Create shape object
	square = new Shape(vertices, indices);

	transform.SetScale(.5f);
	transform.SetPosition(glm::vec2(.25, .25));


    // Texture Setup:

    // First we have to load the texture. We're using freeimage to do this, but you could use any library, or even write your own!

    // Get the image format (we need this to load it)
    FREE_IMAGE_FORMAT fileFormat = FreeImage_GetFileType("../assets/texture.png");
    // Load the file.
    FIBITMAP* bitmap = FreeImage_Load(fileFormat, "../assets/texture.png");
    // Convert the file to 32 bits so we can use it.
    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);


    GLuint texture;
    // Tell openGL to create 1 texture. Store the index of it in our texture variable.
    glGenTextures(1, &texture);

    // Bind our texture to the GL_TEXTURE_2D binding location.
    glBindTexture(GL_TEXTURE_2D, texture);

    // Fill our openGL side texture object.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FreeImage_GetWidth(bitmap32), FreeImage_GetHeight(bitmap32),
        0, GL_BGRA, GL_UNSIGNED_BYTE, static_cast<void*>(FreeImage_GetBits(bitmap32)));

    // We can unload the images now that the texture data has been buffered with opengl
    FreeImage_Unload(bitmap);
    FreeImage_Unload(bitmap32);

    // Set Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    // Unbind the texture.
    glBindTexture(GL_TEXTURE_2D, 0);





    // Create shader program:
    Shader vertexShader, fragmentShader;
    
	// Initialize the shaders using the shader files.
	vertexShader.InitFromFile("../shaders/vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader.InitFromFile("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	// Create a shader program.
    GLuint shaderProgram = glCreateProgram();
	
	// Attach the vertex and fragment shaders to our program.
	vertexShader.AttachTo(shaderProgram);
	fragmentShader.AttachTo(shaderProgram);

	// Build full shader program.
	glLinkProgram(shaderProgram);

	// Get world matrix uniform location
	GLuint worldMatrixUniform = glGetUniformLocation(shaderProgram, "worldMatrix");

    // Get the texture uniform from the shader program.
    GLuint textureUniform = glGetUniformLocation(shaderProgram, "tex");
    





    // In order to render to a texture, we need a frameBuffer.
    // A framebuffer is a buffer that the gpu renders to. By default, OpenGL renders to the backbuffer.

    // Create and bind the framebuffer. This is done exactly the same as it's done for everything else in OpenGL.
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // The bind location form framebuffers is simply named GL_FRAMEBUFFER.

    // The framebuffer actually consists of a few smaller objects.
    // These are primarily textures and renderbuffers, each which have different uses.
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    // Here we're going to create a texture that matches the size of our viewport. (this also gets resized in the viewport resizing code)
    // Instead of passing in data for the texture, we pass in null, which leaves the texture empty.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportDimensions.x, viewportDimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // When we read from the texture, it will be 1 to 1 with the screen, so we shouldnt have any filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // We also need to create a render buffer.
    // A render buffer is similar to a texture, but with less features.
    // When rendering to a texture, we can later use that texture in another draw call, and draw it onto a surface.
    // A render buffer can't do that. The pixel data is temporary, and gets thrown away by the gpu when it's done.

    // We're going to create one here for the depth stencil buffer.
    // This isn't actually required for this example, because we only render one object, but if we were rendering with depth, this would be very important.
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportDimensions.x, viewportDimensions.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    // Finally, we attach both the texture and render buffer to our frame buffer.
    // The texture is attached as a "color attachment". This is where our fragment shader outputs.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    // The render buffer is attached to the depth stencil attachment.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);







    // After we render to our framebuffer, we'll need to do something with the texture.
    // This basic shader program will use vertex and fragment shaders.

    // The vertex shader will create a triangle that covers the entire screen.
    Shader fullscreenVert;
    // The fragment shader will use the previous texture to draw on that triangle.
    Shader invertFrag;

    // Initialize the shaders.
    fullscreenVert.InitFromFile("../shaders/fullscreenVert.glsl", GL_VERTEX_SHADER);
    invertFrag.InitFromFile("../shaders/invertFrag.glsl", GL_FRAGMENT_SHADER);

    // Create program and attach shaders.
    GLuint toScreenProgram = glCreateProgram();
    fullscreenVert.AttachTo(toScreenProgram);
    invertFrag.AttachTo(toScreenProgram);
    glLinkProgram(toScreenProgram);

    // Grab the location of the texture we want to render on screen.
    GLuint screenTextureUniform = glGetUniformLocation(toScreenProgram, "tex");


	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);



		// rotate square
		transform.Rotate(dt);

        // Before rendering, bind the framebuffer. This will cause all following draw calls to render here instead of the backbuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);

		// Set the current shader program.
		glUseProgram(shaderProgram);


        // Bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // This function call sets the texture uniform for our shader program to use the texture at GL_TEXTURE0
        glUniform1i(textureUniform, 0);
		
		// Draw to our offscreen texture.
		square->Draw(transform.GetMatrix(), worldMatrixUniform);


        // Unbind the texture used to render.
        glBindTexture(GL_TEXTURE_2D, 0);




        // Now, unbind the framebuffer. This defaults it to the backbuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render using our full screen shader program
        glUseProgram(toScreenProgram);

        // Bind the output texture from the previous shader program.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glUniform1i(screenTextureUniform, 0);

        // Draw three "vertices" as a triangle. (no buffers required)
        // At this point you should look at the contents of the vertex and fragment shaders.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind everything we were using.
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}

    glDeleteRenderbuffers(1, &renderBuffer);
    glDeleteTextures(1, &screenTexture);
    glDeleteFramebuffers(1, &frameBuffer);

	// Free memory from shader program and individual shaders
	glDeleteProgram(shaderProgram);

    // Free memory from loaded texture.
    glDeleteTextures(1, &texture);

	// Free memory from shape object
	delete square;

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
