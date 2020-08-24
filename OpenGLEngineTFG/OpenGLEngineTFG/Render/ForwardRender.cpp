#include "ForwardRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void ForwardRender::createFrameBuffer(int numLights)
{
	//int SCR_WIDTH = Application::getInstance()->getWIDHT();
	//int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	//// configure framebuffer
	//// ------------------------------
	//glGenFramebuffers(1, &forwardBufferID);
	//glBindFramebuffer(GL_FRAMEBUFFER, forwardBufferID);

	//// create and attach depth buffer (renderbuffer)
	//glGenTextures(1, &forwardBufferDepthID);
	//glBindTexture(GL_TEXTURE_2D, forwardBufferDepthID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//// attach depth texture as FBO's depth buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, forwardBufferID);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, forwardBufferDepthID, 0);

	glGenBuffers(1, &lightsShareBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numLights * sizeof(structLight), 0, GL_DYNAMIC_DRAW);
}

void ForwardRender::draw(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	structLight* pointLights = (structLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	// Pasamos toda la información al buffer de luces que hemos creado
	for (int i = 0; i < lightPosition.size(); i++) {
		structLight& light = pointLights[i];
		light.Position = glm::vec4(lightPosition[i], 0.0f);
		light.Color = glm::vec4(lightColors[i], 0.0f);
		light.IntensityandRadius = glm::vec4(lightIntensity[i], 10.0f, glm::vec2(0.0f));
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, forwardBufferID);
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getForwardLighting()->use();

	int numLights = lightPosition.size();
	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsShareBuffer);
	ShaderManager::getInstance()->getForwardLighting()->setUniform("lightCount", numLights);
	world->DrawObjs(ShaderManager::getInstance()->getForwardLighting(), TypeDraw::ForwardRender);

	// ¿Dibujamos las bolas?
	/*if (lBillboards) {
		glUseProgram(0);
		for (unsigned int i = 0; i < nLights; i++) drawLightBillboard(pointLightsArr[i], 0.15f);
	}*/
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}