#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "NodoScene.h"

class ForwardRender
{
public:
	void createFrameBuffer();
	void draw(
		NodoScene* world, 
		std::vector<glm::vec3> lightPosition,
		std::vector<glm::vec3> lightColors,
		std::vector<float> lightIntensity
	);

private:
	unsigned int forwardBufferID, forwardBufferDepthID;
};

