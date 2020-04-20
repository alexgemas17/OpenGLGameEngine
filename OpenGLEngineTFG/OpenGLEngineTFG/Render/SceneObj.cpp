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
	
	//bool isInFrustum = Application::getInstance()->getMainScene()->camara->isPointInFrustum(*this->getAABB(), ModelViewMatrix);
	//std::cout << "¿Esta el punto en el frustum?: " << isInFrustum << std::endl;

	shader->use();

	if (type == TypeDraw::ShadowMap) {
		shadowMapDraw(shader, MVP);
	}
	else {
		geometryPassDraw(shader, ModelViewMatrix, MVP);
	}

	this->Draw();
}

//------------------------------- PRIVATE -------------------------------
void SceneObj::shadowMapDraw(PagShaderProgram* shader, glm::mat4& MVP)
{
	shader->setUniform("ProjLightModelMatrix", MVP);
}

void SceneObj::geometryPassDraw(PagShaderProgram* shader, glm::mat4& ModelViewMatrix, glm::mat4& MVP)
{
	shader->setUniform("hasNormalTexture", !this->getNormalMapTextures().empty());
	shader->setUniform("hasMetallicTexture", !this->getMetallicTexture().empty());
	shader->setUniform("hasRoughnessTexture", !this->getRoughnessTexture().empty());
	shader->setUniform("hasAOTexture", !this->getAOTexture().empty());

	shader->setUniform("ModelViewMatrix", ModelViewMatrix);
	shader->setUniform("MVP", MVP);
}