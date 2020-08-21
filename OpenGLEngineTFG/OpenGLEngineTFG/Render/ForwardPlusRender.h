#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

// Estructura que usa nuestros shaders para poder compartirlo
struct Light {
	glm::vec3 Position;
	glm::vec3 Color;

	float Linear;
	float Quadratic;
	float Radius;
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
	void updateLighst(std::vector<glm::vec3> lightPosition);
	void draw(NodoScene* world);

private:
	unsigned int lightsShareBuffer, visibleLightIndicesBuffer, dephtFrambuffer, depthText;

	// X and Y work group dimension variables for compute shader
	int workGroupsX = 0;
	int workGroupsY = 0;

	int numLights;

	void depthRender(NodoScene* world);
	void lightCullingRender();
	void lightShader(NodoScene* world);
};

