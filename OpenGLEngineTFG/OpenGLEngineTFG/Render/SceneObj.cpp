#include "SceneObj.h"

SceneObj::SceneObj(): Render(), Model() {}

SceneObj::SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, std::string urlImg): 
	Model(), Render(puntos, index, normales, coordenada_textura, urlImg)
{}

SceneObj::~SceneObj() {}

void SceneObj::UpdateObj(float deltaTime)
{
	//Actualiza el objeto seg�n nuestra intenci�n.
}

void SceneObj::DrawObj(PagShaderProgram* shader)
{
	//Especificamos que vamos a usar el shader que nos han pasado.
	shader->use();

	//Metemos los Uniform correspondientes
	shader->setUniform("ModelMatrix", this->getModelMatrix());

	//Dibujamos
	this->Draw(shader);
}

void SceneObj::DrawObj(PagShaderProgram* shader, glm::mat4 &modelMatrix)
{
	//Especificamos que vamos a usar el shader que nos han pasado.
	shader->use();

	//Metemos los Uniform correspondientes
	shader->setUniform("MatrixMVP", modelMatrix);

	//Dibujamos
	this->Draw(shader);
}