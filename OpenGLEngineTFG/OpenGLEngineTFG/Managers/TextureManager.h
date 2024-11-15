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
	TextureManager();
	unsigned int getIDTexture(std::string urlImage);
	void addIDTexture(std::string urlImage);
	bool LoadTextures();

private:
	std::unordered_map<std::string, unsigned int> hashmap_IDTexture;		//Hash map para los IDs
	std::unordered_map<std::string, unsigned int> hashmap_TextureURL;		//Hash map para los IDs
	std::vector<TextureInfo> textures;
	int indexTexLoaded;

	void InitTextura(TextureInfo &textInf);
	void InitTexturaDevil(TextureInfo& textInf);
};