#include "TextureManager.h"

#include "Loaders/lodepng.h"
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
	if (this->hashmap_IDTexture.find(urlImage) == this->hashmap_IDTexture.end()) {
		TextureInfo textInf;
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

void TextureManager::InitTextura(TextureInfo textInf)
{
	//Generamos la texturaID
	unsigned int idTexture = 0;

	glGenTextures(1, &idTexture);
	glBindTexture(GL_TEXTURE_2D, idTexture);

	textInf.IDTexture = idTexture;

	//Opciones para las texturas
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Obtenemos los datos de la imagen
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, textInf.urlImg);
	if (error)
	{
		std::cout << textInf.urlImg << " cannot be loaded" << std::endl;
		return ;
	}

	//Generamos la imagen
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	this->hashmap_IDTexture.insert(make_pair(textInf.urlImg, textInf.IDTexture));

	image.clear();
}