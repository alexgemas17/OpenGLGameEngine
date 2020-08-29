#include "DeferredShadingRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void DeferredShadingRender::createFrameBuffer(int numLights)
{
	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &fbID);
	glBindFramebuffer(GL_FRAMEBUFFER, fbID);

	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

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

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &lightsShareBuffer);
	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsShareBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numLights * sizeof(structDRLight), 0, GL_DYNAMIC_DRAW);
}

void DeferredShadingRender::initBufferLights(std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
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

void DeferredShadingRender::UpdateLights(std::vector<glm::vec3> lightPosition, int numLights)
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

void DeferredShadingRender::draw(NodoScene* world, unsigned int sceneFBO)
{
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	geometryBufferPass(world, sceneFBO);

	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightingPass(world);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void DeferredShadingRender::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void DeferredShadingRender::geometryBufferPass(NodoScene* world, unsigned int sceneFBO)
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	world->DrawObjs(ShaderManager::getInstance()->getGBuffer(), TypeDraw::GeometryRender);

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void DeferredShadingRender::deferredLightingPass(NodoScene* world)
{
	//No hacemos glBindFramebuffer porque estamos usando el deafult buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	ShaderManager::getInstance()->getDeferredShading()->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPos", Application::getInstance()->getMainScene()->camara->getPosition());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsShareBuffer);

	renderQuad();

	glEnable(GL_DEPTH_TEST);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}