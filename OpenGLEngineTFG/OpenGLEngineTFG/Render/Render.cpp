#include "Render.h"

#include "../Loaders/lodepng.h"
#include "../Application.h"

Render::Render() {}

Render::Render(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, 
	std::string albedoURL, std::string normalURL, std::string materialURL):
	typeRender(DeferredRendering), albedoURL(albedoURL), normalURL(normalURL), materialURL(materialURL)
{
	this->model.puntos = puntos;
	this->model.normales = normales;
	this->model.index = index;
	this->model.coordenada_textura = coordenada_textura;
	this->model.urlImg = albedoURL;
}

Render::Render(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura,
	std::vector<std::string> AlbedoTextures, std::vector<std::string> specularTextures, std::vector<std::string> normalMapTextures) :
	typeRender(DeferredRendering), albedoURL(albedoURL), normalURL(normalURL), materialURL(materialURL)
{
	this->model.puntos = puntos;
	this->model.normales = normales;
	this->model.index = index;
	this->model.coordenada_textura = coordenada_textura;
	this->model.urlImg = albedoURL;

	this->AlbedoTextures = AlbedoTextures;
	this->specularTextures = specularTextures;
	this->normalMapTextures = normalMapTextures;
}

//CONSTRUCTOR BUENO
Render::Render(
	AssimpData* data,
	std::vector<std::string> AlbedoTextures, 
	std::vector<std::string> specularTextures, 
	std::vector<std::string> normalMapTextures,
	std::string MetallicTexture,
	std::string RoughnessTexture,
	std::string AOTexture
) :
	typeRender(DeferredRendering), AlbedoTextures(AlbedoTextures), normalMapTextures(normalMapTextures), 
	specularTextures(specularTextures), MetallicTexture(MetallicTexture),
		RoughnessTexture(RoughnessTexture), AOTexture(AOTexture), dataObj(data)
{}

Render::~Render()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_Puntos);
	glDeleteBuffers(1, &VBO_Normales);
	glDeleteBuffers(1, &CoordTexturaBuffer);
	glDeleteBuffers(1, &VBO_Tangentes);
	glDeleteBuffers(1, &VBO_Bitangentes);

	delete this->dataObj;
}

//------------------------------- PUBLIC -------------------------------
void Render::Init()
{
	InitVAO();
	InitVBO();
	InitIBO();
	InitCoordTextura();
}

void Render::setTypeRender(TypeRender type)
{
	typeRender = type;
}

void Render::Draw()
{
	// Albedo Texture
	if (!AlbedoTextures.empty()) {
		glActiveTexture(GL_TEXTURE0); 
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_albedo", 0);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(AlbedoTextures[0]));
	}

	// Normal Texture
	if (!normalMapTextures.empty()) {
		glActiveTexture(GL_TEXTURE1);
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_normal", 1);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(normalMapTextures[0]));
	}

	// Normal Texture
	if (!MetallicTexture.empty()) {
		glActiveTexture(GL_TEXTURE2);
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_metallic", 2);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(MetallicTexture));
	}

	// Normal Texture
	if (!RoughnessTexture.empty()) {
		glActiveTexture(GL_TEXTURE3);
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_roughness", 3);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(RoughnessTexture));
	}

	// Normal Texture
	if (!AOTexture.empty()) {
		glActiveTexture(GL_TEXTURE4);
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_ao", 4);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(AOTexture));
	}

	//// Specular Texture
	//if (!specularTextures.empty()) {
	//	glActiveTexture(GL_TEXTURE2);
	//	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_AO_Met_Rough", 2);
	//	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(specularTextures[0]));
	//}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 

	// Lines: GL_LINE_STRIP
	glDrawElements(GL_TRIANGLE_STRIP, this->dataObj->indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

//------------------------------- PRIVATE -------------------------------
void Render::InitVAO() 
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_Puntos);
	glGenBuffers(1, &VBO_Normales);
	glGenBuffers(1, &VBO_Tangentes);
	glGenBuffers(1, &VBO_Bitangentes);
	glGenBuffers(1, &CoordTexturaBuffer);
	glGenBuffers(1, &IBO);
}

void Render::InitVBO() 
{
	glBindVertexArray(VAO);

	// -------------- PUNTOS -------------- 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Puntos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 
		sizeof(glm::vec3) / sizeof(GLfloat), 
		GL_FLOAT,
		GL_FALSE, 
		sizeof(glm::vec3), 
		((GLubyte*)NULL + (0))
	);
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(glm::vec3) * this->dataObj->vertices.size(), 
		this->dataObj->vertices.data(), 
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// -------------- NORMALES -------------- 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normales);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 
		sizeof(glm::vec3) / sizeof(GLfloat), 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(glm::vec3), 
		((GLubyte*)NULL + (0))
	);
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(glm::vec3) * this->dataObj->normales.size(), 
		this->dataObj->normales.data(), 
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// -------------- TANGENTES -------------- 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Tangentes);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 
		sizeof(glm::vec3) / sizeof(GLfloat),
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(glm::vec3), 
		((GLubyte*)NULL + (0))
	);
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(glm::vec3) * this->dataObj->tangentes.size(), 
		this->dataObj->tangentes.data(), 
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// -------------- BITANGENTES -------------- 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Bitangentes);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4,
		sizeof(glm::vec3) / sizeof(GLfloat),
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		((GLubyte*)NULL + (0))
	);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * this->dataObj->bitangentes.size(),
		this->dataObj->bitangentes.data(),
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Render::InitIBO() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->dataObj->indices.size(), this->dataObj->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitCoordTextura()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CoordTexturaBuffer);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(glm::vec2) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec2), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->dataObj->coord_textura.size(), this->dataObj->coord_textura.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}