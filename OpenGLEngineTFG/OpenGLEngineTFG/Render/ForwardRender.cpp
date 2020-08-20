#include "ForwardRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void ForwardRender::createFrameBuffer()
{
	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &fbID);
	glBindFramebuffer(GL_FRAMEBUFFER, fbID);

	// create and attach depth buffer (renderbuffer)
	glGenTextures(1, &fbDepthID);
	glBindTexture(GL_TEXTURE_2D, fbDepthID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbDepthID, 0);
}

void ForwardRender::draw(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getForwardLighting()->use();

	// send light relevant uniforms
	for (unsigned int i = 0; i < lightPosition.size(); i++)
	{
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Position", lightPosition[i]);
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Intensity", lightIntensity[i]);

		// update attenuation parameters and calculate radius
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.7;
		const float quadratic = 1.8;
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Linear", linear);
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);

		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		ShaderManager::getInstance()->getForwardLighting()->setUniform("lights[" + std::to_string(i) + "].Radius", radius);

		world->DrawObjs(ShaderManager::getInstance()->getForwardLighting(), TypeDraw::ForwardRender);
	}

	// ¿Dibujamos las bolas?
	/*if (lBillboards) {
		glUseProgram(0);
		for (unsigned int i = 0; i < nLights; i++) drawLightBillboard(pointLightsArr[i], 0.15f);
	}*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}