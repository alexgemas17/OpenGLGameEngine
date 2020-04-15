#include "SceneObj.h"

#include "../Application.h"

SceneObj::SceneObj(): Render(), Model(){}

SceneObj::SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, 
	std::string albedoURL, std::string normalURL, std::string materialURL):
	Model(), Render(puntos, index, normales, coordenada_textura, albedoURL, normalURL, materialURL)
{}

SceneObj::SceneObj(
	std::vector<glm::vec3> puntos, 
	std::vector<GLuint> index, 
	std::vector<glm::vec3> normales,
	std::vector<glm::vec2> coordenada_textura,
	std::vector<std::string> AlbedoTextures, 
	std::vector<std::string> specularTextures, 
	std::vector<std::string> normalMapTextures
):
	Model(), 
	Render(puntos, index, normales, coordenada_textura, AlbedoTextures, specularTextures, normalMapTextures)
{}

SceneObj::SceneObj( 
	AssimpData* data,
	glm::vec3 min,
	glm::vec3 max,
	std::vector<std::string> AlbedoTextures,
	std::vector<std::string> specularTextures,
	std::vector<std::string> normalMapTextures,
	std::string MetallicTexture,
	std::string RoughnessTexture,
	std::string AOTexture
) :
	Model(min, max),
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

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto según nuestra intención.
}

void SceneObj::DrawObjShadowMap(glm::mat4& _modelMatrix)
{
	glm::mat4 ViewMatrix = Application::getInstance()->getMainScene()->camara->getView();
	glm::mat4 ProjMatrix = Application::getInstance()->getMainScene()->camara->getProjection();
	glm::mat4 ModelViewMatrix = ViewMatrix * _modelMatrix;
	bool isInFrustum = Application::getInstance()->getMainScene()->camara->isPointInFrustum(*this->getAABB(), ModelViewMatrix);
	//std::cout << "¿Esta el punto en el frustum?: " << isInFrustum << std::endl;

	if (isInFrustum) {
		ShaderManager::getInstance()->getShadowMap()->use();
		//ShaderManager::getInstance()->getShadowMap()->setUniform("ModelMatrix", _modelMatrix);
		ShaderManager::getInstance()->getShadowMap()->setUniform("ProjLightModelMatrix", ProjMatrix * ModelViewMatrix);

		this->Draw();
	}
}

void SceneObj::DrawObj(PagShaderProgram* shader, glm::mat4& modelMatrix)
{
	glm::mat4 ViewMatrix = Application::getInstance()->getMainScene()->camara->getView();
	glm::mat4 ProjMatrix = Application::getInstance()->getMainScene()->camara->getProjection();
	glm::mat4 ModelViewMatrix = ViewMatrix * modelMatrix;
	bool isInFrustum = Application::getInstance()->getMainScene()->camara->isPointInFrustum(*this->getAABB(), ModelViewMatrix);
	//std::cout << "¿Esta el punto en el frustum?: " << isInFrustum << std::endl;

	if (isInFrustum) {
		shader->use();

		shader->setUniform("hasNormalTexture", !this->getNormalMapTextures().empty());
		shader->setUniform("hasMetallicTexture", !this->getMetallicTexture().empty());
		shader->setUniform("hasRoughnessTexture", !this->getRoughnessTexture().empty());
		shader->setUniform("hasAOTexture", !this->getAOTexture().empty());

		shader->setUniform("ModelViewMatrix", ViewMatrix * modelMatrix);
		shader->setUniform("MVP", ProjMatrix * ModelViewMatrix);

		glm::vec3 vertex = ModelViewMatrix * glm::vec4(vertex, 1.0f);


		this->Draw();
	}
}

void SceneObj::setShaderToPoints(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	//Metemos los Uniform correspondientes
}

void SceneObj::setShaderToWireFrame(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{

	//Metemos los Uniform correspondientes
}

void SceneObj::setShaderToBasicColor(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{

	//Metemos los Uniform correspondientes
}

// Usa el shader de textura para renderizar el objeto
void SceneObj::setShaderToTexture(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	//Especificamos que vamos a usar el shader que nos han pasado.

	//Metemos los Uniform correspondientes
}

// Usa el shader de luces para renderizar el objeto
void SceneObj::setShaderToTextureLight(glm::mat4& _modelMatrix, glm::mat4& _mView, glm::mat4& _mViewProjection)
{
	ShaderManager::getInstance()->getBasicLightShader()->use();

	ShaderManager::getInstance()->getBasicLightShader()->setUniform("mModelView", _mView * _modelMatrix);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("mMVP", _mViewProjection * _modelMatrix);
} 

// Usa el shader de luces para renderizar el objeto
void SceneObj::setShaderDeferredRendering(glm::mat4& _modelMatrix, glm::mat4& _mView, glm::mat4& _mViewProjection)
{
	ShaderManager::getInstance()->getGBuffer()->use();
	
	ShaderManager::getInstance()->getGBuffer()->setUniform("ModelViewMatrix", _mView * _modelMatrix);
	ShaderManager::getInstance()->getGBuffer()->setUniform("MVP", _mViewProjection * _modelMatrix);
}