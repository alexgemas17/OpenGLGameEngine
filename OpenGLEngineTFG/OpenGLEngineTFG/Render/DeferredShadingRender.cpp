#include "DeferredShadingRender.h"

#include "../Application.h"
#include <../glm/gtc/type_ptr.hpp>

void DeferredShadingRender::createFrameBuffer()
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
}

void DeferredShadingRender::draw(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	geometryBufferPass(world);

	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightingPass(world, lightPosition, lightColors, lightIntensity);
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

void DeferredShadingRender::geometryBufferPass(NodoScene* world)
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	ShaderManager::getInstance()->getGBuffer()->use();
	world->DrawObjs(ShaderManager::getInstance()->getGBuffer(), TypeDraw::GeometryRender);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void DeferredShadingRender::deferredLightingPass(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity)
{
	//No hacemos glBindFramebuffer porque estamos usando el deafult buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	ShaderManager::getInstance()->getDeferredShading()->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedoSpec", 2);

	for (unsigned int i = 0; i < lightPosition.size(); i++)
	{
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Position", lightPosition[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);

		// update attenuation parameters and calculate radius
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.7;
		const float quadratic = 1.8;
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Linear", linear);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);

		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Radius", radius);
	}


	//glm::mat4 mView = Application::getInstance()->getMainScene()->camara->getView();
	//glm::mat4 mProj = Application::getInstance()->getMainScene()->camara->getProjection();
	//glm::vec3 cameraPosition = Application::getInstance()->getMainScene()->camara->getPosition();

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPosition", cameraPosition);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("viewInverse", glm::inverse(mView));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("projectionInverse", glm::inverse(mProj));

	//// ------------------------ Light Pass ------------------------
	////NOTA: Las luces están dentro del shader. TO-DO: Ver como pasarla aquí para ir de una en una.
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("NumPointLights", NR_POINT_LIGHTS);

	//// Directional Light 
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.LightDirection", glm::vec3(-1.0f, -1.0f, -1.0f));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.Intensity", 2.0f);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.LightColour", glm::vec3(1.0f));

	//// Point Light
	//for (int i = 0; i < NR_POINT_LIGHTS; i++) {
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Position", lightPosition[i]);
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Intensity", 10.0f);
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].LightColour", glm::vec3(1.0f));
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].AttenuationRadius", 30.0f);
	//}

	renderQuad();

	glEnable(GL_DEPTH_TEST);
}