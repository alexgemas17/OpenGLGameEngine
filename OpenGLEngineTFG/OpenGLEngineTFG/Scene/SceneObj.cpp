#include "SceneObj.h"

#include "../Application.h"

SceneObj::SceneObj(): Render(), Model(){}

SceneObj::SceneObj( 
	AssimpData* data,
	std::vector<std::string> AlbedoTextures,
	std::vector<std::string> specularTextures,
	std::vector<std::string> normalMapTextures
) :
	Model(),
	Render(
		data, 
		AlbedoTextures, 
		specularTextures,
		normalMapTextures
	)
{}

SceneObj::~SceneObj() {}

//------------------------------- PUBLIC -------------------------------

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto seg�n nuestra intenci�n.
}

void SceneObj::SetUniforms(glm::mat4& modelMatrix)
{
	// --------------------------- FORWARD  RENDERING ----------------------------
	ShaderManager::getInstance()->getForwardLighting()->use();
	ShaderManager::getInstance()->getForwardLighting()->setUniform("ModelMatrix", modelMatrix);

	// --------------------------- DEFERRED RENDERING ----------------------------
	ShaderManager::getInstance()->getGBuffer()->use();
	ShaderManager::getInstance()->getGBuffer()->setUniform("ModelMatrix", modelMatrix);

	// --------------------------- FORWARD PLUS RENDERING ----------------------------
	ShaderManager::getInstance()->getDepthShader()->use();
	ShaderManager::getInstance()->getDepthShader()->setUniform("ModelMatrix", modelMatrix);

	ShaderManager::getInstance()->getForwardPlusLighting()->use();
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("ModelMatrix", modelMatrix);
}

void SceneObj::DrawObj(PagShaderProgram* shader, const TypeDraw& type)
{
	glm::mat4 ViewMatrix = Application::getInstance()->getMainScene()->camara->getView();
	glm::mat4 ProjMatrix = Application::getInstance()->getMainScene()->camara->getProjection();

	shader->use();

	shader->setUniform("ViewMatrix", ViewMatrix);
	shader->setUniform("ProjectionMatrix", ProjMatrix);

	switch (type)
	{
	case TypeDraw::ForwardRender:
		forwardDraw(shader, ViewMatrix, ProjMatrix);
		break;
	case TypeDraw::ForwardPlusRender:
		forwardPlusDraw(shader, ViewMatrix, ProjMatrix);
		break;
	}

	if (type != TypeDraw::DepthRender) {
		this->Draw(shader);
		return;
	}

	this->DrawDepth();
}

//------------------------------- PRIVATE -------------------------------
void SceneObj::forwardDraw(PagShaderProgram* shader, glm::mat4& ViewMatrix, glm::mat4& ProjMatrix)
{
	shader->setUniform("viewPos", Application::getInstance()->getMainScene()->camara->getPosition());
}

void SceneObj::forwardPlusDraw(PagShaderProgram* shader, glm::mat4& ViewMatrix, glm::mat4& ProjMatrix)
{
	shader->setUniform("viewPos", Application::getInstance()->getMainScene()->camara->getPosition());
}