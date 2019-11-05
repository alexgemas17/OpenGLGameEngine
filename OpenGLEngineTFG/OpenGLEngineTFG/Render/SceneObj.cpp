#include "SceneObj.h"

SceneObj::SceneObj(): Render(), Model() {}

SceneObj::SceneObj(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> color, std::vector<glm::vec2> coordenada_textura, std::string urlImg): 
	puntos(puntos), index(index), color(color), coordenada_textura(coordenada_textura), urlImg(urlImg), Model(), Render(puntos, index, color, coordenada_textura, urlImg)
{}

SceneObj::~SceneObj() {}

void SceneObj::DrawObj(PagShaderProgram* shader)
{
	//Especificamos que vamos a usar el shader que nos han pasado.
	shader->use();

	//Metemos los Uniform correspondientes
	shader->setUniform("ModelMatrix", this->getModelMatrix());

	//Dibujamos
	this->Draw(shader);
}