#pragma once

#include <glm.hpp>
#include <vector>
#include "Model.h"
#include "Render.h"

#include "../PagShaderProgram.h"

class SceneObj : public Render, public Model
{
public:
	SceneObj();
	SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> color, std::vector<glm::vec2> coordenada_textura, std::string urlImg);

	~SceneObj();

	void UpdateObj(float deltaTime);
	void DrawObj(PagShaderProgram* shader);
	void DrawObj(PagShaderProgram* shader, glm::mat4 &modelMatrix);

private:
	std::vector<glm::vec3> puntos;
	std::vector<GLuint> index;
	std::vector<glm::vec3> color;
	std::vector<glm::vec2> coordenada_textura;
	std::string urlImg;
};
