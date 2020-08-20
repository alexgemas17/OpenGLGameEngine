#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

class ForwardPlusRender
{
public:
	void createFrameBuffer();
	void draw();

private:
	unsigned int fbID;

};

