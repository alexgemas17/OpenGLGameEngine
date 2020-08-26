#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "../Render/SceneObj.h"

class Cube
{
public:
	Cube(float size);
	void Draw();

private:
	unsigned int cubeVAO, cubeVBO;
	unsigned int cubeTexture;

	std::string texture = "Data\\Textures\\transparent_window.png";

	void LoadCubeMap();
};

