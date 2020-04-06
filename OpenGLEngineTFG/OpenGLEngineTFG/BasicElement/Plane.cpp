#include "Plane.h"
#include "../Loaders/AssimpLoader.h"

#include "../Application.h"

Plane::Plane(TypeFloor type, float size)
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

	std::vector<glm::vec3> tangente = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	std::vector<glm::vec3> bitangente = {
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
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
	data->bitangentes = bitangente;
	data->tangentes = tangente;
	data->indices = indices;

	std::vector<std::string> albedoText;
	std::vector<std::string> specText;
	std::vector<std::string> normalText;
	if (type == Grass1) {
		albedoText = { path + Grass1_albedo };
		specText = { path + Grass1_albedo };
		normalText = { path + Grass1_normal };
	}
	else {
		albedoText = { path + Narrow1_albedo };
		specText = { path + Narrow1_albedo };
		normalText = { path + Narrow1_normal };
	}

	Application::getInstance()->getTextureManager()->addIDTexture(albedoText[0]);
	Application::getInstance()->getTextureManager()->addIDTexture(normalText[0]);

	this->obj = new SceneObj(
		data, 
		albedoText,
		specText, 
		normalText, 
		albedoText[0],
		albedoText[0],
		albedoText[0]
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