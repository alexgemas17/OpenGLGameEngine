#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"
#include "../PagShaderProgram.h"

class Render {
public:
	Render();

	Render(
		AssimpData* data,
		std::vector<std::string> AlbedoTextures,
		std::vector<std::string> specularTextures,
		std::vector<std::string> normalMapTextures
		);

	~Render();

	void Init(); 
	void DrawDepth();
	void Draw(PagShaderProgram* shader);

	void setAlbedo(std::string urlImg) { this->AlbedoTextures.push_back(urlImg); }
	std::vector<std::string> getAlbedoTextures() { return this->AlbedoTextures; }
	std::vector<std::string> getSpecularTextures() { return this->specularTextures; }
	std::vector<std::string> getNormalMapTextures() { return this->normalMapTextures; }

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

	//Inits según el tipo que se haya especificado
	void InitVAOInterleaved();
	void InitVAO();
	void InitIBO();
};