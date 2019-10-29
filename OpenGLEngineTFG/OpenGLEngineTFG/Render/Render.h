#pragma one

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"

class Render {
public:
	Render(std::vector<glm::vec3> _vertices);

	void Init();
	void Draw(PagShaderProgram* shader);

private:

	unsigned int VAO;
	unsigned int VBO; 
};