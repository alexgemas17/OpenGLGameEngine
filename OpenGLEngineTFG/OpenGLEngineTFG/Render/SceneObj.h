#pragma once

#include <glm.hpp>
#include <vector>
#include <string>
#include "Model.h"
#include "Render.h"

#include "../Managers/ShaderManager.h"

class SceneObj : public Render, public Model
{
public:
	SceneObj();
	SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, std::string urlImg);
	~SceneObj();

	void UpdateObj(float deltaTime);
	void DrawObj(glm::mat4 &modelMatrix, glm::mat4& mView, glm::mat4& mViewProjection);

private:
	std::string urlImg;

	void setShaderToPoints(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToWireFrame(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToBasicColor(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToTexture(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToTextureLight(glm::mat4& modelMatrix, glm::mat4& mView, glm::mat4& mViewProjection);
};
