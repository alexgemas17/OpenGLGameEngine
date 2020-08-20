#include "Render.h"

#include "../Loaders/lodepng.h"
#include "../Application.h"

Render::Render() {}

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
	AlbedoTextures(AlbedoTextures), 
	specularTextures(specularTextures), 
	dataObj(data)
{
	if (!normalMapTextures.empty())
		this->normalMapTextures = normalMapTextures;

	if (MetallicTexture != "")
		this->MetallicTexture = MetallicTexture;

	if (RoughnessTexture != "")
		this->RoughnessTexture = RoughnessTexture;

	if (AOTexture != "")
		this->AOTexture = AOTexture;
}

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
	InitVAOInterleaved();
	InitIBO();
}

void Render::Draw()
{
	if (AlbedoTextures.empty())
		return;

	// Albedo Texture
	glActiveTexture(GL_TEXTURE0);
	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_albedo", 0);
	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(AlbedoTextures[0]));

	// Normal Texture
	/*glActiveTexture(GL_TEXTURE1);
	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_normal", 1);
	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(normalMapTextures[0]));*/

	//// Normal Texture
	//if (!MetallicTexture.empty()) {
	//	glActiveTexture(GL_TEXTURE2);
	//	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_metallic", 2);
	//	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(MetallicTexture));
	//}

	//// Normal Texture
	//if (!RoughnessTexture.empty()) {
	//	glActiveTexture(GL_TEXTURE3);
	//	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_roughness", 3);
	//	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(RoughnessTexture));
	//}

	//// Normal Texture
	//if (!AOTexture.empty()) {
	//	glActiveTexture(GL_TEXTURE4);
	//	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_ao", 4);
	//	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(AOTexture));
	//}

	//// Specular Texture
	//if (!specularTextures.empty()) {
	//	glActiveTexture(GL_TEXTURE2);
	//	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_AO_Met_Rough", 2);
	//	glBindTexture(GL_TEXTURE_2D, Application::getInstance()->getTextureManager()->getIDTexture(specularTextures[0]));
	//}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
	glDrawElements(GL_TRIANGLES, this->dataObj->indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

//------------------------------- PRIVATE -------------------------------
void Render::InitVAOInterleaved()
{
	// Preprocess the mesh data in the format that was specified
	std::vector<float> dataVAO;
	for (unsigned int i = 0; i < dataObj->vertices.size(); i++) {

		dataVAO.push_back(dataObj->vertices[i].x);
		dataVAO.push_back(dataObj->vertices[i].y);
		dataVAO.push_back(dataObj->vertices[i].z);

		if (dataObj->normales.size() > 0) {
			dataVAO.push_back(dataObj->normales[i].x);
			dataVAO.push_back(dataObj->normales[i].y);
			dataVAO.push_back(dataObj->normales[i].z);
		}
		if (dataObj->coord_textura.size() > 0) {
			dataVAO.push_back(dataObj->coord_textura[i].x);
			dataVAO.push_back(dataObj->coord_textura[i].y);
		}
		if (dataObj->tangentes.size() > 0) {
			dataVAO.push_back(dataObj->tangentes[i].x);
			dataVAO.push_back(dataObj->tangentes[i].y);
			dataVAO.push_back(dataObj->tangentes[i].z);
		}
		if (dataObj->bitangentes.size() > 0) {
			dataVAO.push_back(dataObj->bitangentes[i].x);
			dataVAO.push_back(dataObj->bitangentes[i].y);
			dataVAO.push_back(dataObj->bitangentes[i].z);
		}
	}

	// Compute the component count
	unsigned int bufferComponentCount = 0;
	if (dataObj->vertices.size() > 0)
		bufferComponentCount += 3;

	if (dataObj->normales.size() > 0)
		bufferComponentCount += 3;

	if (dataObj->coord_textura.size() > 0)
		bufferComponentCount += 2;

	if (dataObj->tangentes.size() > 0)
		bufferComponentCount += 3;

	if (dataObj->bitangentes.size() > 0)
		bufferComponentCount += 3;

	size_t stride = bufferComponentCount * sizeof(float);
	size_t offset = 0;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Puntos);
	glBufferData(GL_ARRAY_BUFFER, dataVAO.size() * sizeof(float), &dataVAO[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	offset += 3 * sizeof(float);

	if (dataObj->normales.size() > 0) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += 3 * sizeof(float);
	}

	if (dataObj->coord_textura.size() > 0) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += 2 * sizeof(float);
	}

	if (dataObj->tangentes.size() > 0) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += 3 * sizeof(float);
	}

	if (dataObj->bitangentes.size() > 0) {
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += 3 * sizeof(float);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

void Render::InitIBO() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->dataObj->indices.size(), this->dataObj->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}