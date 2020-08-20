#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

class DeferredShadingRender
{
public:
	void createFrameBuffer();
	void draw();

private:
	unsigned int fbID, fbDepthID;
	unsigned int quadVAO = 0, quadVBO = 0;
	unsigned int gPosition, gNormal, gAlbedo, gMaterialInfo;

	void renderQuad();
	void geometryBufferPass();
	void deferredLightingPass();
};

