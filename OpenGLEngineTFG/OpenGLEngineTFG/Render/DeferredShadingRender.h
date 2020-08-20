#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

class DeferredShadingRender
{
public:
	void createFrameBuffer();
	void draw(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity);

	unsigned int getGBufferID() { return this->fbID; }

private:
	unsigned int fbID, fbDepthID;
	unsigned int quadVAO = 0, quadVBO = 0;
	unsigned int gPosition, gNormal, gAlbedo, gMaterialInfo;

	void renderQuad();
	void geometryBufferPass(NodoScene* world);
	void deferredLightingPass(NodoScene* world, std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity);
};

