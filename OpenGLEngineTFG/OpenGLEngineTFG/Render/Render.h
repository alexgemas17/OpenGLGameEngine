#pragma one

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"

struct ImageData {
	std::vector<unsigned char> image;
	unsigned width, height;
};

class Render {
public:
	Render(std::vector<glm::vec3> _vertices);
	~Render();

	void Init();
	void Draw(PagShaderProgram* shader);

private:

	unsigned int VAO;
	unsigned int VBO;

	unsigned int EBO;

	unsigned int texture;

	ImageData loadImage(std::string url);
};