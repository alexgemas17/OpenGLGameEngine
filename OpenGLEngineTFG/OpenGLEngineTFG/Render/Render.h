#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"

struct ImageData {
	std::vector<unsigned char> image;
	unsigned width, height;
};

struct ModelData {
	std::vector<glm::vec3> puntos;
	std::vector<GLuint> index;
	std::vector<glm::vec2> coordenada_textura;
	std::string urlImg;
};

enum TypeRender { Puntos, PuntosTextura, PuntosNormalesTextura, PuntosNormalesTexturaBump};

class Render {
public:
	Render();
	Render(float vertices[], unsigned int indices[]);
	Render(float vertices[], unsigned int indices[], std::string urlImg);
	Render(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> color, std::vector<glm::vec2> coordenada_textura, std::string urlImg);

	~Render();

	void Init();

	void Draw(PagShaderProgram* shader);

private:

	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	unsigned int texture;

	unsigned int CoordTexturaBuffer;

	TypeRender typeRender;

	float* vertices;
	unsigned int* indices;
	ImageData data;
	std::string urlImg;
	ModelData model;

	ImageData loadImage(std::string url);

	//Inits según el tipo que se haya especificado
	void InitVAO();
	void InitVBO();
	void InitIBO();
	void InitCoordTextura();
	void InitTextura();

	void algo();
};