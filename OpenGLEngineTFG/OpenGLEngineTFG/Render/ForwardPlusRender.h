#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Scene/NodoScene.h"

struct LightStruct {
	glm::vec4 Position;
	glm::vec4 Color;
	glm::vec4 IntensityandRadius;
};

struct VisibleIndex {
	int index;
};

class ForwardPlusRender
{
public:
	void createFrameBuffer(int numLights);
	void initBufferLights(
		std::vector<glm::vec3> lightPosition,
		std::vector<glm::vec3> lightColors,
		std::vector<float> lightIntensity
	);
	void UpdateLights(std::vector<glm::vec3> lightPosition, int numLights);
	void draw(NodoScene* world, unsigned int sceneFBO);

	float getWorkGroupsX() { return this->workGroupsX; }

private:
	unsigned int lightsShareBuffer, visibleLightIndicesBuffer, dephtFrambuffer, depthText;

	// X and Y work group dimension variables for compute shader
	float workGroupsX = 0;
	float workGroupsY = 0;

	int numLights;

	void depthRender(NodoScene* world, unsigned int sceneFBO);
	void lightCullingRender();
	void lightShader(NodoScene* world);
};

