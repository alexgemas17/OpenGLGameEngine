#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

struct structLight {
	glm::vec4 Position;
	glm::vec4 Color;
	glm::vec4 IntensityandRadius;
};

class ForwardRender
{
public:
	void createFrameBuffer(int numLights);
	void initBufferLights(std::vector<glm::vec3> lightPosition, std::vector<glm::vec3> lightColors, std::vector<float> lightIntensity);
	void UpdateLights(std::vector<glm::vec3> lightPosition);
	void draw(NodoScene* world);
	unsigned int getLightShareBuffer() { return this->lightsShareBuffer; }

private:
	unsigned int forwardBufferID, forwardBufferDepthID, lightsShareBuffer;
};

