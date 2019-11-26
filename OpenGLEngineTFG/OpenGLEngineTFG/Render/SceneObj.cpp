#include "SceneObj.h"

SceneObj::SceneObj(): Render(), Model(), urlImg("") {}

SceneObj::SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, std::string urlImg): 
	urlImg(urlImg), Model(), Render(puntos, index, normales, coordenada_textura, urlImg)
{}

SceneObj::~SceneObj() {}

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto según nuestra intención.
}

void SceneObj::DrawObj(glm::mat4 &modelMatrix, glm::mat4 mViewProjection)
{
	//Modo textura
	if (this->urlImg != "") {
		//Especificamos que vamos a usar el shader que nos han pasado.
		ShaderManager::getInstance()->getTextureShader()->use();

		//Metemos los Uniform correspondientes
		ShaderManager::getInstance()->getTextureShader()->setUniform("MatrixMVP", modelMatrix);
	}
	//Modo plano
	else {
		//ShaderManager::getInstance()->getBasicShader()->use();

		////Metemos los Uniform correspondientes
		//ShaderManager::getInstance()->getBasicShader()->setUniform("MatrixMVP", modelMatrix);


		ShaderManager::getInstance()->getBasicLightShader()->use();

		ShaderManager::getInstance()->getBasicLightShader()->setUniform("mModel", modelMatrix);
		ShaderManager::getInstance()->getBasicLightShader()->setUniform("mMVP", mViewProjection * modelMatrix);
	}

	//Dibujamos
	this->Draw();
}