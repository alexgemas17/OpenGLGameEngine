#include "Cube.h"

Cube::Cube(float size)
{
	this->puntos = {
		glm::vec3(  size, -size, -size),
		glm::vec3(	size, -size,  size),
		glm::vec3( -size, -size,  size),
		glm::vec3( -size, -size, -size),
		glm::vec3(	size,  size, -size),
		glm::vec3(	size,  size,  size),
		glm::vec3( -size,  size,  size),
		glm::vec3( -size,  size, -size)
	};

	this->normales = {
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),

		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),
		glm::vec3(0.0f,  0.0f, -1.0f),

		glm::vec3(-1.0f, -0.0f, -0.0f),
		glm::vec3(-1.0f, -0.0f, -0.0f),
		glm::vec3(-1.0f, -0.0f, -0.0f),

		glm::vec3(-1.0f, -0.0f, -0.0f),
		glm::vec3(-1.0f, -0.0f, -0.0f),
		glm::vec3(-1.0f, -0.0f, -0.0f),

		glm::vec3( -0.0f, -0.0f,  1.0f),
		glm::vec3( -0.0f, -0.0f,  1.0f),
		glm::vec3( -0.0f, -0.0f,  1.0f),

		glm::vec3( -0.0f,  0.0f,  1.0f),
		glm::vec3( -0.0f,  0.0f,  1.0f),
		glm::vec3( -0.0f,  0.0f,  1.0f),

		glm::vec3(  1.0f, -0.0f,  0.0f),
		glm::vec3(  1.0f, -0.0f,  0.0f),
		glm::vec3(  1.0f, -0.0f,  0.0f),

		glm::vec3(	1.0f,  0.0f,  0.0f),
		glm::vec3(	1.0f,  0.0f,  0.0f),
		glm::vec3(	1.0f,  0.0f,  0.0f),

		glm::vec3(  0.0f,  1.0f, -0.0f),
		glm::vec3(  0.0f,  1.0f, -0.0f),
		glm::vec3(  0.0f,  1.0f, -0.0f),
		
		glm::vec3(  0.0f,  1.0f, -0.0f),
		glm::vec3(  0.0f,  1.0f, -0.0f),
		glm::vec3(  0.0f,  1.0f, -0.0f),

		glm::vec3( -0.0f, -1.0f,  0.0f),
		glm::vec3( -0.0f, -1.0f,  0.0f),
		glm::vec3( -0.0f, -1.0f,  0.0f),
		
		glm::vec3( -0.0f, -1.0f,  0.0f),
		glm::vec3( -0.0f, -1.0f,  0.0f),
		glm::vec3( -0.0f, -1.0f,  0.0f),
	};

	this->coord_textura = {
		glm::vec2(0.748573, 0.750412),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	this->indices = {
		4,0,3, 0xFFFFFFFF,
		4,3,7, 0xFFFFFFFF,
		2,6,7, 0xFFFFFFFF,
		2,7,3, 0xFFFFFFFF,
		1,5,2, 0xFFFFFFFF,
		5,6,2, 0xFFFFFFFF,
		0,4,1, 0xFFFFFFFF,
		4,5,1, 0xFFFFFFFF,
		4,7,5, 0xFFFFFFFF,
		7,6,5, 0xFFFFFFFF,
		0,1,2, 0xFFFFFFFF,
		0,2,3, 0xFFFFFFFF
//f 5/1/1  1/2/1  4/3/1
//f 5/1/1  4/3/1  8/4/1
//f 3/5/2  7/6/2  8/7/2
//f 3/5/2  8/7/2  4/8/2
//f 2/9/3  6/10/3 3/5/3
//f 6/10/4 7/6/4  3/5/4
//f 1/2/5  5/1/5  2/9/5
//f 5/1/6  6/10/6 2/9/6
//f 5/1/7  8/11/7 6/10/7
//f 8/11/7 7/12/7 6/10/7
//f 1/2/8  2/9/8  3/13/8
//f 1/2/8  3/13/8 4/14/8
};

	this->obj = new SceneObj(this->puntos, this->indices, this->normales, this->coord_textura, "");
}

Cube::~Cube()
{
	delete obj;
}

SceneObj* Cube::getSceneObj()
{
	return this->obj;
}
