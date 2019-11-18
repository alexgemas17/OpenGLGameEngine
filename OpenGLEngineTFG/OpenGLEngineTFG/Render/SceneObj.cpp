#include "SceneObj.h"

SceneObj::SceneObj(): Render(), Model() {}

SceneObj::SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, std::string urlImg): 
	Model(), Render(puntos, index, normales, coordenada_textura, urlImg)
{}

SceneObj::~SceneObj() {}

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto según nuestra intención.
}

void SceneObj::DrawObj(glm::mat4 &modelMatrix)
{
	//Especificamos que vamos a usar el shader que nos han pasado.
	//shader->use();
	ShaderManager::getInstance()->getTextureShader()->use();

	//Metemos los Uniform correspondientes
	//shader->setUniform("MatrixMVP", modelMatrix);
	ShaderManager::getInstance()->getTextureShader()->setUniform("MatrixMVP", modelMatrix);

	//Dibujamos
	this->Draw();
}