#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Loaders/stb_imagen.h"

#include "../Loaders/lodepng.h"
#include "../Application.h"
#include <iostream>

unsigned int TextureManager::getIDTexture(std::string urlImage)
{
	if (this->hashmap_IDTexture.find(urlImage) != this->hashmap_IDTexture.end()) 
		return this->hashmap_IDTexture[urlImage]; 
	else
		return -1;
}

void TextureManager::addIDTexture(std::string urlImage)
{
	std::unordered_map<std::string, unsigned int>::const_iterator it = this->hashmap_IDTexture.find(urlImage);
	if (it == this->hashmap_IDTexture.end()) {
		TextureInfo textInf;
		textInf.IDTexture = -1;
		textInf.urlImg = urlImage;
		
		this->textures.push_back(textInf);
	}
}

void TextureManager::LoadTextures()
{
	for (auto &textInf : this->textures)
	{
		InitTextura(textInf);
	}
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
			formatImg = GL_RGBA;
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