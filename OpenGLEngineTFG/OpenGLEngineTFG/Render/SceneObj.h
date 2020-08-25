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
	DepthRender,
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
		std::vector<std::string> normalMapTextures
	);

	~SceneObj();

	void UpdateObj(float deltaTime);

	void SetUniforms(
		glm::mat4 &modelMatrix
	);
	
	void DrawObj(
		PagShaderProgram* shader,
		const TypeDraw& type
	);

private:
	void shadowMapDraw(
		PagShaderProgram* shader, 
		glm::mat4& MVP
	);

	void depthRender(
		PagShaderProgram* shader,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);

	void forwardDraw(
		PagShaderProgram* shader,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);

	void geometryDraw(
		PagShaderProgram* shader,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);

	void forwardPlusDraw(
		PagShaderProgram* shader,
		glm::mat4& ViewMatrix,
		glm::mat4& ProjMatrix
	);
};
