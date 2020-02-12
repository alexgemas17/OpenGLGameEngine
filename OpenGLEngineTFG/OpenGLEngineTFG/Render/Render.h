#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

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

enum TypeRender { Points, Wireframe, BasicColor, Texture, TextureLight };

class Render {
public:
	Render();
	Render(float vertices[], unsigned int indices[]);
	Render(float vertices[], unsigned int indices[], std::string urlImg);
	Render(
		std::vector<glm::vec3> puntos, 
		std::vector<GLuint> index, 
		std::vector<glm::vec3> normales, 
		std::vector<glm::vec2> coordenada_textura, 
		std::string albedoURL, std::string normalURL, std::string materialURL);

	~Render();

	void Init();
	void Draw();

	void setTypeRender(TypeRender type);
	TypeRender getTypeRender();

private:

	unsigned int VAO;
	unsigned int VBO_Puntos;
	unsigned int VBO_Normales;
	unsigned int IBO;
	unsigned int texture;

	unsigned int CoordTexturaBuffer;

	TypeRender typeRender;

	float* vertices;
	unsigned int* indices;
	ImageData data;
	ModelData model;

	std::string albedoURL;
	std::string normalURL;
	std::string materialURL;

	ImageData loadImage(std::string url);

	//Inits según el tipo que se haya especificado
	void InitVAO();
	void InitVBO();
	void InitIBO();
	void InitCoordTextura();
	void InitTextura();
};