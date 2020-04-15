#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "Model.h"
#include "Render.h"

#include "../Loaders/AssimpLoader.h"
#include "../Managers/ShaderManager.h"

class SceneObj : public Render, public Model
{
public:
	SceneObj();
	SceneObj(
		std::vector<glm::vec3> puntos, 
		std::vector<GLuint> index, 
		std::vector<glm::vec3> normales,
		std::vector<glm::vec2> coordenada_textura, 
		std::string albedoURL, 
		std::string normalURL, 
		std::string materialURL
	);

	SceneObj(
		std::vector<glm::vec3> puntos,
		std::vector<GLuint> index,
		std::vector<glm::vec3> normales,
		std::vector<glm::vec2> coordenada_textura,
		std::vector<std::string> AlbedoTextures,
		std::vector<std::string> specularTextures,
		std::vector<std::string> normalMapTextures
	);
	
	SceneObj( 
		AssimpData* data,
		glm::vec3 min,
		glm::vec3 max,
		std::vector<std::string> AlbedoTextures,
		std::vector<std::string> specularTextures,
		std::vector<std::string> normalMapTextures,
		std::string MetallicTexture,
		std::string RoughnessTexture,
		std::string AOTexture
	);

	~SceneObj();

	void UpdateObj(float deltaTime);

	void DrawObjShadowMap(
		glm::mat4& modelMatrix
	);

	void DrawObj(
		PagShaderProgram* shader,
		glm::mat4 &modelMatrix
	);

private:
	void setShaderToPoints(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToWireFrame(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToBasicColor(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToTexture(glm::mat4& modelMatrix, glm::mat4& mViewProjection);
	void setShaderToTextureLight(glm::mat4& modelMatrix, glm::mat4& mView, glm::mat4& mViewProjection);
	void setShaderDeferredRendering(glm::mat4& _modelMatrix, glm::mat4& _mView, glm::mat4& _mViewProjection);
};
