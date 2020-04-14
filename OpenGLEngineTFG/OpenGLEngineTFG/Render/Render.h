#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"

struct ImageData {
	std::vector<unsigned char> image;
	unsigned width, height;
};

struct ModelData {
	std::vector<glm::vec3> puntos;
	std::vector<glm::vec3> normales;
	std::vector<GLuint> index;
	std::vector<glm::vec2> coordenada_textura;
	std::string urlImg;
};

enum TypeRender { 
	Points,	//Borrar?
	Wireframe,	//Borrar?
	BasicColor,	//Borrar?
	SSAO, 
	ShadowMap,
	DeferredRendering
};

class Render {
public:
	Render();

	Render(
		std::vector<glm::vec3> puntos, 
		std::vector<GLuint> index, 
		std::vector<glm::vec3> normales, 
		std::vector<glm::vec2> coordenada_textura, 
		std::string albedoURL, std::string normalURL, std::string materialURL);

	Render(
		std::vector<glm::vec3> puntos, 
		std::vector<GLuint> index, 
		std::vector<glm::vec3> normales, 
		std::vector<glm::vec2> coordenada_textura,
		std::vector<std::string> AlbedoTextures, 
		std::vector<std::string> specularTextures, 
		std::vector<std::string> normalMapTextures);

	//CONSTRUCTOR BUENO
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

	void setTypeRender(TypeRender type);
	TypeRender getTypeRender() { return this->typeRender; }

	std::vector<std::string> getAlbedoTextures() { return this->AlbedoTextures; }
	std::vector<std::string> getSpecularTextures() { return this->specularTextures; }
	std::vector<std::string> getNormalMapTextures() { return this->normalMapTextures; }
	std::string getMetallicTexture() { return this->MetallicTexture; }
	std::string getRoughnessTexture() { return this->RoughnessTexture; }
	std::string getAOTexture() { return this->AOTexture; }

	AssimpData* dataObj;

private:

	unsigned int VAO;
	unsigned int VBO_Puntos;			//VertexAttribArray = 0
	unsigned int VBO_Normales;			//VertexAttribArray = 1
	unsigned int CoordTexturaBuffer;	//VertexAttribArray = 2
	unsigned int VBO_Tangentes;			//VertexAttribArray = 3
	unsigned int VBO_Bitangentes;		//VertexAttribArray = 4
	unsigned int IBO;
	unsigned int texture;


	TypeRender typeRender;


	//BORRAR?
	float* vertices;
	unsigned int* indices;
	ImageData data;
	ModelData model;

	std::string albedoURL;
	std::string normalURL;
	std::string materialURL;

	std::vector<std::string> AlbedoTextures;
	std::vector<std::string> specularTextures;
	std::vector<std::string> normalMapTextures;
	std::string MetallicTexture;
	std::string RoughnessTexture;
	std::string AOTexture;

	//Inits según el tipo que se haya especificado
	void InitVAO();
	void InitVBO();
	void InitIBO();
	void InitCoordTextura();
};