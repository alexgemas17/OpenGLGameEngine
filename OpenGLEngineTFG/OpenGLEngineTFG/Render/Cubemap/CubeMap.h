#pragma once

#include <string>
#include <vector>

enum TypeSky {
	Type1
};

class CubeMap
{
public:

	CubeMap(TypeSky type);
	void Draw();

private:
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;

	std::string Top = "Data\\Textures\\Skybox\\FluffballDayTop.png";
	std::string Front = "Data\\Textures\\Skybox\\FluffballDayFront.png";
	std::string Back = "Data\\Textures\\Skybox\\FluffballDayBack.png";
	std::string Bottom = "Data\\Textures\\Skybox\\FluffballDayBottom.png";
	std::string Right  = "Data\\Textures\\Skybox\\FluffballDayLeft.png";
	std::string Left = "Data\\Textures\\Skybox\\FluffballDayRight.png";
	
	void LoadCubeMap(std::vector<std::string> SkyTextures);
};

