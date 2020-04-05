#include "Plane.h"
#include "../Loaders/AssimpLoader.h"

#include "../Application.h"

Plane::Plane(float size)
{
	AssimpLoader* loader = new AssimpLoader();

	std::string path = Application::getInstance()->getPath();

	//Datos del objeto
	std::vector<glm::vec3> puntos = {
		glm::vec3( 1.0f, 0.0f,  1.0f),
		glm::vec3(-1.0f, 0.0f,  1.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f),
		glm::vec3(1.0f,  0.0f, -1.0f)
	};

	std::vector<glm::vec3> normales = {
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	std::vector<glm::vec2> coord_textura = {
		glm::vec2(1.0f,  1.0f),
		glm::vec2(0.0f,  1.0f),
		glm::vec2(0.0f,  0.0f),
		glm::vec2(1.0f,  0.0f)
	};

	std::vector<GLuint> indices = {
		1,0,2,1,0xFFFFFFFF,
		0,3,2,0,0xFFFFFFFF
	};

	//float planeVertices[] = {
	//	// positions            // normals         // texcoords
	//	 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	//	-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	//	-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

	//	 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	//	-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
	//	 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	//};

	AssimpData* data = new AssimpData;
	data->vertices = puntos;
	data->normales = normales;
	data->coord_textura = coord_textura;
	data->bitangentes = normales;
	data->tangentes = normales;
	data->indices = indices;

	std::vector<std::string> albedoText = { path + floor_albedo };
	std::vector<std::string> specText = { path + floor_albedo };
	std::vector<std::string> normalText = { path + floor_normal };

	Application::getInstance()->getTextureManager()->addIDTexture(path + floor_albedo);
	Application::getInstance()->getTextureManager()->addIDTexture(path + floor_normal);

	this->obj = new SceneObj(
		data, 
		albedoText,
		specText, 
		normalText, 
		path + floor_albedo,
		path + floor_albedo, 
		path + floor_albedo
	);
	this->obj->Scale(size, size, size);

	delete loader;
}

Plane::~Plane()
{
	delete obj;
}

SceneObj* Plane::getSceneObj()
{
	return this->obj;
}