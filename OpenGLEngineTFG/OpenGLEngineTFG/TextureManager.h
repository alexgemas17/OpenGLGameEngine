#pragma once

#include <GL\glew.h>
#include <string>
#include <vector>
#include <unordered_map> 

struct TextureInfo {
	unsigned int IDTexture;
	std::string urlImg;
};

class TextureManager
{
public:

	unsigned int getIDTexture(std::string urlImage);
	void addIDTexture(std::string urlImage);
	void LoadTextures();

private:
	std::unordered_map<std::string, unsigned int> hashmap_IDTexture;		//Hash map para los IDs
	std::vector<TextureInfo> textures;

	void InitTextura(TextureInfo textInf);
};

