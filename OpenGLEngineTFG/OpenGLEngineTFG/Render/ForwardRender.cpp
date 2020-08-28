#include "ForwardRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void ForwardRender::createFrameBuffer(int numLights)
{
	glGenBuffers(1, &lightsShareBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numLights * sizeof(structLight), 0, GL_DYNAMIC_DRAW);
}

void ForwardRender::initBufferLights(std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	if (lightsShareBuffer == 0) {
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	LightStruct* pointLights = (LightStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	// Pasamos toda la información al buffer de luces que hemos creado
	for (int i = 0; i < lightPosition.size(); i++) {
		LightStruct& light = pointLights[i];
		light.Position = glm::vec4(lightPosition[i], 0.0f);
		light.Color = glm::vec4(lightColors[i], 0.0f);
		light.IntensityandRadius = glm::vec4(lightIntensity[i], 10.0f, glm::vec2(0.0f));
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardRender::UpdateLights(std::vector<glm::vec3> lightPosition, int numLights)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	LightStruct* pointLights = (LightStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < numLights; i++) {
		LightStruct& light = pointLights[i];
		light.Position = glm::vec4(lightPosition[i], 1.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardRender::draw(NodoScene* world)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, forwardBufferID);
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsShareBuffer);
	world->DrawObjs(ShaderManager::getInstance()->getForwardLighting(), TypeDraw::ForwardRender);

	// ¿Dibujamos las bolas?
	/*if (lBillboards) {
		glUseProgram(0);
		for (unsigned int i = 0; i < nLights; i++) drawLightBillboard(pointLightsArr[i], 0.15f);
	}*/

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}