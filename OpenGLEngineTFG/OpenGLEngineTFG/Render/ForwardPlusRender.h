#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

// Estructura que usa nuestros shaders para poder compartirlo
//struct LightStruct {
//	glm::vec3 Position;
//	glm::vec3 Color;
//	float Intensity;
//
//	float Linear;
//	float Quadratic;
//	float Radius;
//};

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
	void updateLights(std::vector<glm::vec3> lightPosition);
	void draw(NodoScene* world);

private:
	unsigned int lightsShareBuffer, visibleLightIndicesBuffer, dephtFrambuffer, depthText;

	// X and Y work group dimension variables for compute shader
	float workGroupsX = 0;
	float workGroupsY = 0;

	int numLights;

	void depthRender(NodoScene* world);
	void lightCullingRender();
	void lightShader(NodoScene* world);
};

