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
	std::string roughess;
	std::string metallic;
	std::string ao;
	if (type == Grass1) {
		albedoText = { path + Grass1_albedo };
		specText = { path + Grass1_albedo };
		normalText = { path + Grass1_normal };
		roughess = path + Grass1_roughess;
		metallic = path + Grass1_metallic;
		ao = path + Grass1_AO;
	}
	if (type == Stone1) {
		albedoText = { path + Stone1_albedo };
		specText = { path + Stone1_albedo };
		normalText = { path + Stone1_normal };
		roughess = path + Stone1_roughess;
		metallic = path + Stone1_metallic;
		ao = path + Stone1_AO;
	}
	else {
		albedoText = { path + Narrow1_albedo };
		specText = { path + Narrow1_albedo };
		normalText = { path + Narrow1_normal };
		roughess = path + Narrow1_roughess;
		metallic = path + Narrow1_metallic;
		ao = path + Narrow1_AO;
	}

	Application::getInstance()->getTextureManager()->addIDTexture(albedoText[0]);
	Application::getInstance()->getTextureManager()->addIDTexture(normalText[0]);
	Application::getInstance()->getTextureManager()->addIDTexture(roughess);
	Application::getInstance()->getTextureManager()->addIDTexture(metallic);
	Application::getInstance()->getTextureManager()->addIDTexture(ao);

	this->obj = new SceneObj(
		data, 
		albedoText,
		specText, 
		normalText, 
		roughess,
		metallic,
		ao
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