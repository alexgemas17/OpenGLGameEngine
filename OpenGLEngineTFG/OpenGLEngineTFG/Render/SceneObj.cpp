#include "SceneObj.h"

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
	std::vector<std::string> AlbedoTextures,
	std::vector<std::string> specularTextures,
	std::vector<std::string> normalMapTextures
) :
	Model(),
	Render(
		data, 
		AlbedoTextures, 
		specularTextures,
		normalMapTextures)
{}

SceneObj::~SceneObj() {}

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto según nuestra intención.
}

void SceneObj::DrawObj(glm::mat4 &_modelMatrix, glm::mat4& _mView, glm::mat4& _mViewProjection)
{
	switch (getTypeRender())
	{
	case Points:
		setShaderToPoints(_modelMatrix, _mViewProjection);
		break;

	case Wireframe:
		setShaderToWireFrame(_modelMatrix, _mViewProjection);
		break;

	case Texture:
		setShaderToTexture(_modelMatrix, _mViewProjection);
		break;

	case BasicColor:
		setShaderToBasicColor(_modelMatrix, _mViewProjection);
		break;

	case TextureLight:
		setShaderToTextureLight(_modelMatrix, _mView, _mViewProjection);
		break;
	case DeferredRendering:
		setShaderDeferredRendering(_modelMatrix, _mView, _mViewProjection);
		break;
	}

	//Dibujamos
	this->Draw();
}

void SceneObj::setShaderToPoints(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	ShaderManager::getInstance()->getBasicShader()->use();

	//Metemos los Uniform correspondientes
	ShaderManager::getInstance()->getBasicShader()->setUniform("MatrixMVP", mViewProjection * modelMatrix);
}

void SceneObj::setShaderToWireFrame(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	ShaderManager::getInstance()->getBasicShader()->use();

	//Metemos los Uniform correspondientes
	ShaderManager::getInstance()->getBasicShader()->setUniform("MatrixMVP", mViewProjection * modelMatrix);
}

void SceneObj::setShaderToBasicColor(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	ShaderManager::getInstance()->getBasicShader()->use();

	//Metemos los Uniform correspondientes
	ShaderManager::getInstance()->getBasicShader()->setUniform("MatrixMVP", mViewProjection * modelMatrix);
}

// Usa el shader de textura para renderizar el objeto
void SceneObj::setShaderToTexture(glm::mat4& modelMatrix, glm::mat4& mViewProjection)
{
	//Especificamos que vamos a usar el shader que nos han pasado.
	ShaderManager::getInstance()->getTextureShader()->use();

	//Metemos los Uniform correspondientes
	ShaderManager::getInstance()->getTextureShader()->setUniform("MatrixMVP", modelMatrix);
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

	ShaderManager::getInstance()->getGBuffer()->setUniform("mModel", _modelMatrix);
	ShaderManager::getInstance()->getGBuffer()->setUniform("mMVP", _mViewProjection * _modelMatrix);
}