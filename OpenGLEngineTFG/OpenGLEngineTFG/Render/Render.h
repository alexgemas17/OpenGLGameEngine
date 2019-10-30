#pragma one

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"

struct ImageData {
	std::vector<unsigned char> image;
	unsigned width, height;
};

enum TypeRender { Puntos, PuntosTextura, PuntosNormalesTextura, PuntosNormalesTexturaBump};

class Render {
public:
	Render(float vertices[], unsigned int indices[]);
	Render(float vertices[], unsigned int indices[], std::string urlImg);
	Render(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> color, std::vector<glm::vec2> coordenada_textura, std::string urlImg);

	~Render();

	void Init();

	void Draw(PagShaderProgram* shader);

private:

	unsigned int VAO;
	unsigned int VBO;

	unsigned int EBO;

	unsigned int texture;

	TypeRender typeRender;

	float* vertices;
	unsigned int* indices;
	ImageData data;
	std::string urlImg;

	ImageData loadImage(std::string url);

	//Inits según el tipo que se haya especificado
	void InitVAO();
	void InitPuntos();
	void InitTextura();

	void oldInit();
	std::vector<glm::vec3> puntos;
};