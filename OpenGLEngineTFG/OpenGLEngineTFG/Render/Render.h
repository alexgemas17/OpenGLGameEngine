#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"

class Render {
public:
	Render();

	Render(
		AssimpData* data,
		std::vector<std::string> AlbedoTextures,
		std::vector<std::string> specularTextures,
		std::vector<std::string> normalMapTextures,
		std::string MetallicTexture,
		std::string RoughnessTexture,
		std::string AOTexture
		);

	~Render();

	void Init();
	void Draw();

	std::vector<std::string> getAlbedoTextures() { return this->AlbedoTextures; }
	std::vector<std::string> getSpecularTextures() { return this->specularTextures; }
	std::vector<std::string> getNormalMapTextures() { return this->normalMapTextures; }
	std::string getMetallicTexture() { return this->MetallicTexture; }
	std::string getRoughnessTexture() { return this->RoughnessTexture; }
	std::string getAOTexture() { return this->AOTexture; }

	AssimpData* dataObj;

private:

	unsigned int VAO;
	unsigned int IBO;
	unsigned int VBO_Puntos;			//VertexAttribArray = 0
	unsigned int VBO_Normales;			//VertexAttribArray = 1
	unsigned int CoordTexturaBuffer;	//VertexAttribArray = 2
	unsigned int VBO_Tangentes;			//VertexAttribArray = 3
	unsigned int VBO_Bitangentes;		//VertexAttribArray = 4

	std::vector<std::string> AlbedoTextures;
	std::vector<std::string> specularTextures;
	std::vector<std::string> normalMapTextures;
	std::string MetallicTexture;
	std::string RoughnessTexture;
	std::string AOTexture;

	//Inits según el tipo que se haya especificado
	void InitVAOInterleaved();
	void InitVAO();
	void InitIBO();
};