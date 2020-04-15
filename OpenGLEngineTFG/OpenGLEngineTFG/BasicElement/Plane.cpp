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

	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);

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
		albedoText = { Grass1_albedo };
		specText = { Grass1_specular };
		normalText = { Grass1_normal };
		roughess = Grass1_roughess;
		metallic = Grass1_metallic;
		ao = Grass1_AO;
	}
	if (type == Stone1) {
		albedoText = { Stone1_albedo };
		specText = { Stone1_specular };
		normalText = { Stone1_normal };
		roughess = Stone1_roughess;
		metallic = Stone1_metallic;
		ao = Stone1_AO;
	}
	else {
		albedoText = {  Narrow1_albedo };
		specText = { Narrow1_specular };
		normalText = { Narrow1_normal };
		roughess = Narrow1_roughess;
		metallic = Narrow1_metallic;
		ao = Narrow1_AO;
	}

	albedoText[0] = path + albedoText[0];

	if (specText[0] != "")
		specText[0] = path + specText[0];

	if (normalText[0] != "")
		normalText[0] = path + normalText[0];
	
	if (roughess != "")
		roughess = path + roughess;

	if (metallic != "")
		metallic = path + metallic;

	if (ao != "")
		ao = path + ao;


	Application::getInstance()->getTextureManager()->addIDTexture(albedoText[0]);
	Application::getInstance()->getTextureManager()->addIDTexture(normalText[0]);
	Application::getInstance()->getTextureManager()->addIDTexture(roughess);
	Application::getInstance()->getTextureManager()->addIDTexture(metallic);
	Application::getInstance()->getTextureManager()->addIDTexture(ao);

	this->obj = new SceneObj(
		data, 
		min,
		max,
		albedoText,
		specText, 
		normalText,
		metallic,
		roughess,
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