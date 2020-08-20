#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "Model.h"
#include "Render.h"

#include "../Loaders/AssimpLoader.h"
#include "../Managers/ShaderManager.h"

const enum class TypeDraw {
	ForwardRender,
	GeometryRender,
	ForwardPlusRender,
	ShadowMap,
};

class SceneObj : public Render, public Model
{
public:
	SceneObj();

	SceneObj( 
		AssimpData* data,
		std::vector<std::string> AlbedoTextures,
		std::vector<std::string> specularTextures,
		std::vector<std::string> normalMapTextures,
		std::string MetallicTexture,
		std::string RoughnessTexture,
		std::string AOTexture
	);

	~SceneObj();

	void UpdateObj(float deltaTime);

	void DrawObj(
		PagShaderProgram* shader,
		glm::mat4 &modelMatrix,
		const TypeDraw& type
	);

private:
	void shadowMapDraw(
		PagShaderProgram* shader, 
		glm::mat4& MVP
	);

	void forwardDraw(
		PagShaderProgram* shader,
		glm::mat4& modelMatrix,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);

	void geometryDraw(
		PagShaderProgram* shader,
		glm::mat4& ModelViewMatrix,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);

	void forwardPlusDraw(
		PagShaderProgram* shader,
		glm::mat4& modelMatrix,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);
};
