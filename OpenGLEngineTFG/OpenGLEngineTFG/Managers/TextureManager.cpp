#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Loaders/stb_imagen.h"

#include "il.h"
#include "ilu.h"
#include "ilut.h"

#include "../Loaders/lodepng.h"
#include "../Application.h"
#include <iostream>

TextureManager::TextureManager(): indexTexLoaded(0)
{
	ilInit();
	iluInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);
}

unsigned int TextureManager::getIDTexture(std::string urlImage)
{
	if (this->hashmap_IDTexture.find(urlImage) != this->hashmap_IDTexture.end()) 
		return this->hashmap_IDTexture[urlImage]; 
	else
		return -1;
}

void TextureManager::addIDTexture(std::string urlImage)
{
	std::unordered_map<std::string, unsigned int>::const_iterator it = this->hashmap_TextureURL.find(urlImage);
	if (it == this->hashmap_TextureURL.end()) {
		TextureInfo textInf;
		textInf.IDTexture = -1;
		textInf.urlImg = urlImage;
		
		this->hashmap_TextureURL.insert(make_pair(urlImage, 0));
		this->textures.push_back(textInf);
	}
}

bool TextureManager::LoadTextures()
{
	InitTexturaDevil(this->textures[indexTexLoaded]);

	indexTexLoaded++;
	if (indexTexLoaded >= textures.size())
		return true;

	return false;
}

// --------------------------- FUNCIONES PRIVADAS ----------------------------
void TextureManager::InitTextura(TextureInfo &textInf)
{
	//Generamos la texturaID
	unsigned int idTexture = 0;

	glGenTextures(1, &idTexture);
	textInf.IDTexture = idTexture;

	int width, height, nrComponents; 
	GLenum formatImg = GL_RGBA;
	unsigned char* data = stbi_load(textInf.urlImg.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		if (nrComponents == 1)
			formatImg = GL_RED;
		else if (nrComponents == 3)
			formatImg = GL_RGB;
		else if (nrComponents == 4)
			formatImg = GL_RGBA16F;
	}
	else {
		std::cout << textInf.urlImg << " cannot be loaded" << std::endl;
		
		// Si no carga la textura, cargamos una textura genérica.
		std::string url = Application::getInstance()->getPath() + no_texture;
		data = stbi_load(url.c_str(), &width, &height, &nrComponents, 0);
	}

	//Generamos la imagen 
	glBindTexture(GL_TEXTURE_2D, idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, formatImg, width, height, 0, formatImg, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Opciones para las texturas
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->hashmap_IDTexture.insert(make_pair(textInf.urlImg, textInf.IDTexture));

	std::cout << textInf.urlImg << " be loaded successfully" << std::endl;

	stbi_image_free(data);
}

void TextureManager::InitTexturaDevil(TextureInfo& textInf)
{
	unsigned int idTexture = 0;
	int width, height, componentsPerPixel;

	const char* filename = textInf.urlImg.c_str();
	GLubyte* textureData = stbi_load(filename, &width, &height, &componentsPerPixel, 0);

	if (!textureData) {
		std::cout << "Couldn't load image: " << filename << std::endl;
		return;
	}

	// Generate a texture ID and bind to it
	glGenTextures(1, &idTexture);
	glBindTexture(GL_TEXTURE_2D, idTexture);

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
		std::cout << "There was an error loading the texture: " << textInf.urlImg << std::endl;
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

	textInf.IDTexture = idTexture;
	this->hashmap_IDTexture.insert(make_pair(textInf.urlImg, textInf.IDTexture));
	std::cout << textInf.urlImg << " be loaded successfully" << std::endl;


	stbi_image_free(textureData);


	//ILuint dTexId = 0;
	//ilGenImages(1, &dTexId);
	//ilBindImage(dTexId);

	//if (ilLoadImage(textInf.urlImg.c_str())) {

	//	if (ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE)) {
	//		glGenTextures(1, &idTexture);
	//		textInf.IDTexture = idTexture;
	//		glBindTexture(GL_TEXTURE_2D, idTexture);

	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//		GLuint* pixels = (GLuint*) ilGetData();
	//		GLuint width = (GLuint) ilGetInteger(IL_IMAGE_WIDTH);
	//		GLuint height = (GLuint) ilGetInteger(IL_IMAGE_HEIGHT);

	//		// Bind DevIL texture to OpenGL texture...
	//		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, pixels);
	//		glGenerateMipmap(GL_TEXTURE_2D);

	//		this->hashmap_IDTexture.insert(make_pair(textInf.urlImg, textInf.IDTexture));

	//		std::cout << textInf.urlImg << " be loaded successfully" << std::endl;
	//	}
	//	else {
	//		std::cout << textInf.urlImg << " cannot be converted to unsigned byte" << std::endl;
	//	}
	//}
	//else {
	//	std::cout << textInf.urlImg << " cannot be loaded" << std::endl;
	//}

	//ilDeleteImages(1, &dTexId);
}