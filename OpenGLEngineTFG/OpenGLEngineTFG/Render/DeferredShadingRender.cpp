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

	// Info PBR: metallic, roughness, ao
	glGenTextures(1, &gMaterialInfo);
	glBindTexture(GL_TEXTURE_2D, gMaterialInfo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMaterialInfo, 0);

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
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredShadingRender::draw()
{
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	geometryBufferPass();

	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightingPass();
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

void DeferredShadingRender::geometryBufferPass()
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	ShaderManager::getInstance()->getGBuffer()->use();
	//this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getGBuffer(), TypeDraw::GeometryRender);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void DeferredShadingRender::deferredLightingPass()
{
	////No hacemos glBindFramebuffer porque estamos usando el deafult buffer
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_DEPTH_TEST);

	////Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	//ShaderManager::getInstance()->getDeferredShading()->use();

	////glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, gPosition);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);*/
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, DepthGBuffer);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("depthTexture", 0);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, gNormal);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, gAlbedo);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedo", 2);

	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, gMaterialInfo);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gMaterialInfo", 3);

	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("ssaoTexture", 4);

	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_2D, DepthShadowMap);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("shadowMap", 5);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("lightSpaceMatrix", lightSpaceMatrix);

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPosition", this->camara->getPosition());
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
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Position", lightPositions[i]);
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Intensity", 10.0f);
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].LightColour", glm::vec3(1.0f));
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].AttenuationRadius", 30.0f);
	//}

	////Spot Light 
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.Position", glm::vec3(0.0, 10.0, 5.0));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.LightDirection", glm::vec3(0.0f, -1.0f, 0.0f));

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.Intensity", 100.0f);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.LightColour", glm::vec3(1.0f, 0.52, 0.0));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.AttenuationRadius", 50.0f);

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.CutOff", glm::cos(glm::radians(20.0f)));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.OuterCutOff", glm::cos(glm::radians(0.0f)));

	//renderQuad();

	//glEnable(GL_DEPTH_TEST);
}