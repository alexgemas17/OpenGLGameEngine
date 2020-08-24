#include "ForwardPlusRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void ForwardPlusRender::createFrameBuffer(int numLights)
{
	this->numLights = numLights;
	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// So we need to create a depth map FBO
	// This will be used in the depth pass
	// Create a depth map frame buffer object and texture
	glGenFramebuffers(1, &dephtFrambuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, dephtFrambuffer);

	glGenTextures(1, &depthText);
	glBindTexture(GL_TEXTURE_2D, depthText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, dephtFrambuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthText, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// -------------------------------------------------------------------------------------------------------------------
	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)
	workGroupsX = (SCR_WIDTH + (SCR_WIDTH % 16)) / 16;
	workGroupsY = (SCR_HEIGHT + (SCR_HEIGHT % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;

	// Generate our shader storage buffers
	glGenBuffers(1, &lightsShareBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numLights * sizeof(LightStruct), 0, GL_DYNAMIC_DRAW);

	// Bind visible light indices buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * 1024, 0, GL_STATIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * numLights, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardPlusRender::initBufferLights( std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	if (lightsShareBuffer == 0) {
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	LightStruct* pointLights = (LightStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	// Pasamos toda la información al buffer de luces que hemos creado
	for (int i = 0; i < this->numLights; i++) {
		LightStruct& light = pointLights[i];
		light.Position = glm::vec4(lightPosition[i], 0.0f);
		light.Color = glm::vec4(lightColors[i], 0.0f);
		light.IntensityandRadius = glm::vec4(lightIntensity[i], 10.0f, glm::vec2(0.0f));
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardPlusRender::updateLights(std::vector<glm::vec3> lightPosition)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	LightStruct* pointLights = (LightStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < lightPosition.size(); i++) {
		LightStruct& light = pointLights[i];
		light.Position = glm::vec4(lightPosition[i], 1.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardPlusRender::draw(NodoScene* world)
{
	// 1. Hacemos una pasada para almanecar la posición de los objetos
	depthRender(world);

	// Calculamos el impacto de las luces sobre la escena según los grid
	lightCullingRender();

	// 3. Iluminamos como siempre la escena usando la información del paso anterior.
	lightShader(world);

}

// -------------------------------------------------------------------------------
void ForwardPlusRender::depthRender(NodoScene* world)
{
	// Bind the depth map's frame buffer and draw the depth map to it
	glBindFramebuffer(GL_FRAMEBUFFER, dephtFrambuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	ShaderManager::getInstance()->getDepthShader()->use();
	world->DrawObjs(ShaderManager::getInstance()->getDepthShader(), TypeDraw::DepthRender);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPlusRender::lightCullingRender()
{
	ShaderManager::getInstance()->getLightingCulling()->use();
	
	glm::mat4 mView = Application::getInstance()->getMainScene()->camara->getView();
	glm::mat4 mProj = Application::getInstance()->getMainScene()->camara->getProjection();
	ShaderManager::getInstance()->getLightingCulling()->setUniform("projection", mView);
	ShaderManager::getInstance()->getLightingCulling()->setUniform("view", mProj);

	ShaderManager::getInstance()->getLightingCulling()->setUniform("lightCount", numLights);

	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();
	ShaderManager::getInstance()->getLightingCulling()->setUniform("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	ShaderManager::getInstance()->getLightingCulling()->setUniform("depthMap", 4);
	glBindTexture(GL_TEXTURE_2D, depthText);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsShareBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(workGroupsX, workGroupsY, 1);
	// Unbind the depth map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

void ForwardPlusRender::lightShader(NodoScene* world)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getForwardPlusLighting()->use();

	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("numberOfTilesX", workGroupsX);
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("lightCount", numLights);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsShareBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);
	world->DrawObjs(ShaderManager::getInstance()->getForwardPlusLighting(), TypeDraw::ForwardPlusRender);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}