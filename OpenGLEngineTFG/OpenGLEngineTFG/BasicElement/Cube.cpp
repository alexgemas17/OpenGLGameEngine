#include "Cube.h"

#include "../Application.h"
#include "../Loaders/stb_imagen.h"
#include <GL\glew.h>
#include <iostream>

float cubeVertices[] = {
	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
	0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
	1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

Cube::Cube(float size)
{
    // cube VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

    LoadCubeMap();
}

void Cube::Draw()
{
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
}


void Cube::LoadCubeMap()
{
	int width, height, componentsPerPixel;

	std::string img = Application::getInstance()->getPath() + texture;
	const char* filename = img.c_str();
	GLubyte* textureData = stbi_load(filename, &width, &height, &componentsPerPixel, 0);

	if (!textureData) {
		std::cout << "Couldn't load image: " << filename << std::endl;
		return;
	}

	// Generate a texture ID and bind to it
	glGenTextures(1, &cubeTexture);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);

	if (componentsPerPixel == 4) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
			0,                // Mipmap level (0 being the top level i.e. full size)
			GL_RGBA,          // Internal format
			width,       // Width of the texture
			height,      // Height of the texture,
			0,                // Border in pixels
			GL_RGBA,          // Data format
			GL_UNSIGNED_BYTE, // Type of texture data
			textureData);     // The image data to use for this texture
	}
	else if (componentsPerPixel == 3) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
			0,                // Mipmap level (0 being the top level i.e. full size)
			GL_RGB,          // Internal format
			width,       // Width of the texture
			height,      // Height of the texture,
			0,                // Border in pixels
			GL_RGB,          // Data format
			GL_UNSIGNED_BYTE, // Type of texture data
			textureData);     // The image data to use for this texture
	}
	else if (componentsPerPixel == 1) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
			0,                // Mipmap level (0 being the top level i.e. full size)
			GL_RED,          // Internal format
			width,       // Width of the texture
			height,      // Height of the texture,
			0,                // Border in pixels
			GL_RED,          // Data format
			GL_UNSIGNED_BYTE, // Type of texture data
			textureData);     // The image data to use for this texture
	}

	if (componentsPerPixel == 4 || componentsPerPixel == 3 || componentsPerPixel == 1) {
		// Specify our minification and magnification filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Check for OpenGL texture creation errors
	GLenum glError = glGetError();
	if (glError) {
		std::cout << "There was an error loading the texture: " << img << std::endl;
		switch (glError) {
		case GL_INVALID_ENUM:
			std::cout << "Invalid enum." << std::endl;
			break;

		case GL_INVALID_VALUE:
			std::cout << "Invalid value." << std::endl;
			break;

		case GL_INVALID_OPERATION:
			std::cout << "Invalid operation." << std::endl;

		default:
			std::cout << "Unrecognised GLenum." << std::endl;
			break;
		}
	}

	stbi_image_free(textureData);
}