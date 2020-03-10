#include "Render.h"

#include "../Loaders/lodepng.h"
#include "../Application.h"

Render::Render(): VAO(0), VBO_Puntos(0), VBO_Normales(0), IBO(0), texture(0) {}

Render::Render(float _vertices[], unsigned int _indices[]):
	VAO(0), VBO_Puntos(0), VBO_Normales(0), IBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(Points)
{}

Render::Render(float _vertices[], unsigned int _indices[], std::string imgUrl) :
	VAO(0), VBO_Puntos(0), VBO_Normales(0), IBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(DeferredRendering), albedoURL(imgUrl)
{
	Application::getInstance()->getTextureManager()->addIDTexture(albedoURL);
}

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

Render::~Render()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_Puntos);
	glDeleteBuffers(1, &VBO_Normales);
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
	this->typeRender = type;
}

TypeRender Render::getTypeRender()
{
	return this->typeRender;
}

void Render::Draw()
{
	//shader->use(); --> NOTA: Se llama antes en el DrawObj del SceneObj.

	//if (this->albedoURL != "") glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(this->albedoURL));
	//else glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(Application::getInstance()->getPath() + no_texture));

	// Albedo Texture
	if (!AlbedoTextures.empty()) {
		glActiveTexture(GL_TEXTURE0); 
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_diffuse", 0);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(AlbedoTextures[0]));
	}

	// Specular Texture
	if (!specularTextures.empty()) {
		glActiveTexture(GL_TEXTURE1); 
		ShaderManager::getInstance()->getGBuffer()->setUniform("texture_specular", 1);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(specularTextures[0]));
	}

	// Normal Texture
	//glActiveTexture(GL_TEXTURE2);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("texture_normal", 2);
	//glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(normalMapTextures[0]));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 

	// Lines: GL_LINE_STRIP
	glDrawElements(GL_TRIANGLE_STRIP, this->model.index.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

//------------------------------- PRIVATE -------------------------------
void Render::InitVAO() 
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_Puntos);
	glGenBuffers(1, &VBO_Normales);
	glGenBuffers(1, &CoordTexturaBuffer);
	glGenBuffers(1, &IBO);
}

void Render::InitVBO() 
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	// Enlazamos el VBO puntos
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Puntos);

	// Puntos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(glm::vec3) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec3), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->model.puntos.size(), this->model.puntos.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Enlazamos el VBO normales
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normales);

	// Normales
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(glm::vec3) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec3), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->model.normales.size(), this->model.normales.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitIBO() 
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	//Enlazamos el VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->model.index.size(), this->model.index.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitCoordTextura()
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	// Enlazamos el CoordTexturaBuffer
	glBindBuffer(GL_ARRAY_BUFFER, CoordTexturaBuffer);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(glm::vec2) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec2), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->model.coordenada_textura.size(), this->model.coordenada_textura.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}