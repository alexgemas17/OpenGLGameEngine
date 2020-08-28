#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

struct structDRLight {
	glm::vec4 Position;
	glm::vec4 Color;
	glm::vec4 IntensityandRadius;
};

class DeferredShadingRender
{
public:
	void createFrameBuffer(int numLights); 
	void initBufferLights(std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity);
	void UpdateLights(std::vector<glm::vec3> lightPosition, int numLights);
	void draw(NodoScene* world);

	unsigned int getGBufferID() { return this->fbID; }

private:
	unsigned int fbID, fbDepthID, lightsShareBuffer;
	unsigned int quadVAO = 0, quadVBO = 0;
	unsigned int gPosition, gNormal, gAlbedo, gMaterialInfo;

	void renderQuad();
	void geometryBufferPass(NodoScene* world);
	void deferredLightingPass(NodoScene* world);
};

