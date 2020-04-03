#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

struct ObjFile {
	glm::vec3 position;
	glm::vec3 scale;
	float angleRotation;
	glm::vec3 rotationDirection;

	std::string obj;
	std::string textureURL;			// textureURL-> Albedo, Normal
	
	//PBR
	std::string metallic_texture;	
	std::string roughness_texture;
	std::string ao_texture;
};

class FileLoader
{
public:
	std::vector<ObjFile> getMainScene();
	void readFromFile(std::string urlFile);

private:
	std::vector<ObjFile> mainScene;

	glm::vec3 getVectorFromStr(std::string str);
};

