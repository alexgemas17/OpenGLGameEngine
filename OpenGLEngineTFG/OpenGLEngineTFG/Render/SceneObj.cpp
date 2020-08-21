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


	switch (type)
	{
	case TypeDraw::ForwardRender:
		forwardDraw(shader, modelMatrix, ViewMatrix, ProjMatrix);
		break;
	case TypeDraw::GeometryRender:
		geometryDraw(shader, modelMatrix, ViewMatrix, ProjMatrix);
		break;
	case TypeDraw::ForwardPlusRender:
		forwardPlusDraw(shader, modelMatrix, ViewMatrix, ProjMatrix);
		break;
	case TypeDraw::DepthRender:
		glm::mat4 MVP = ProjMatrix * ViewMatrix * modelMatrix;
		depthRender(shader, MVP);
		break;
	}

	this->Draw();
}

//------------------------------- PRIVATE -------------------------------
void SceneObj::shadowMapDraw(PagShaderProgram* shader, glm::mat4& MVP)
{
	shader->setUniform("ProjLightModelMatrix", MVP);
}

void SceneObj::depthRender(PagShaderProgram* shader, glm::mat4& MVP)
{
	shader->setUniform("MVP", MVP);
}

void SceneObj::forwardDraw(PagShaderProgram* shader, glm::mat4& modelMatrix, glm::mat4& ViewMatrix, glm::mat4& ProjMatrix)
{
	shader->setUniform("model", modelMatrix);
	shader->setUniform("view", ViewMatrix);
	shader->setUniform("projection", ProjMatrix);
}

void SceneObj::geometryDraw(PagShaderProgram* shader, glm::mat4& modelMatrix, glm::mat4& ViewMatrix, glm::mat4& ProjMatrix)
{
	shader->setUniform("model", modelMatrix);
	shader->setUniform("view", ViewMatrix);
	shader->setUniform("projection", ProjMatrix);
}

void SceneObj::forwardPlusDraw(PagShaderProgram* shader, glm::mat4& modelMatrix, glm::mat4& ViewMatrix, glm::mat4& ProjMatrix)
{
	shader->setUniform("model", modelMatrix);
	shader->setUniform("view", ViewMatrix);
	shader->setUniform("projection", ProjMatrix);
}