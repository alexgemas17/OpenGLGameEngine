#include "SceneObj.h"

#include "../Application.h"

SceneObj::SceneObj(): Render(), Model(){}

SceneObj::SceneObj( 
	AssimpData* data,
	std::vector<std::string> AlbedoTextures,
	std::vector<std::string> specularTextures,
	std::vector<std::string> normalMapTextures,
	std::string MetallicTexture,
	std::string RoughnessTexture,
	std::string AOTexture
) :
	Model(),
	Render(
		data, 
		AlbedoTextures, 
		specularTextures,
		normalMapTextures,
		MetallicTexture,
		RoughnessTexture,
		AOTexture
	)
{}

SceneObj::~SceneObj() {}

//------------------------------- PUBLIC -------------------------------

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto según nuestra intención.
}

void SceneObj::DrawObj(PagShaderProgram* shader, glm::mat4& modelMatrix, const TypeDraw& type)
{
	glm::mat4 ViewMatrix = Application::getInstance()->getMainScene()->camara->getView();
	glm::mat4 ProjMatrix = Application::getInstance()->getMainScene()->camara->getProjection();
	glm::mat4 ModelViewMatrix = ViewMatrix * modelMatrix;
	glm::mat4 MVP = ProjMatrix * ModelViewMatrix;

	switch (type)
	{
	case TypeDraw::ForwardRender:
		forwardDraw(shader, ModelViewMatrix, MVP);
		break;
	case TypeDraw::GeometryRender:
		geometryDraw(shader, ModelViewMatrix, MVP);
		break;
	case TypeDraw::ForwardPlusRender:
		forwardPlusDraw(shader, ModelViewMatrix, MVP);
		break;
	}

	this->Draw();

	/*if (type == TypeDraw::ShadowMap) {
		glm::mat4 lightSpaceMatrix = Application::getInstance()->getMainScene()->lightSpaceMatrix;
		glm::mat4 matrixShadow = lightSpaceMatrix * modelMatrix;
		shadowMapDraw(shader, matrixShadow);
	}*/
}

//------------------------------- PRIVATE -------------------------------
void SceneObj::shadowMapDraw(PagShaderProgram* shader, glm::mat4& MVP)
{
	shader->setUniform("ProjLightModelMatrix", MVP);
}

void SceneObj::forwardDraw(PagShaderProgram* shader, glm::mat4& ModelViewMatrix, glm::mat4& MVP)
{
	shader->setUniform("ModelViewMatrix", ModelViewMatrix);
	shader->setUniform("MVP", MVP);
}

void SceneObj::geometryDraw(PagShaderProgram* shader, glm::mat4& ModelViewMatrix, glm::mat4& MVP)
{
	shader->setUniform("ModelViewMatrix", ModelViewMatrix);
	shader->setUniform("MVP", MVP);
}

void SceneObj::forwardPlusDraw(PagShaderProgram* shader, glm::mat4& ModelViewMatrix, glm::mat4& MVP)
{
	shader->setUniform("ModelViewMatrix", ModelViewMatrix);
	shader->setUniform("MVP", MVP);
}