#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "../Render/SceneObj.h"

enum TypeFloor {
	Grass1, Narrow1, Stone1
};

class Plane
{
public:
	Plane(TypeFloor type, float size);
	~Plane();

	SceneObj* getSceneObj();

private:
	//Obj
	SceneObj* obj;

	std::string Narrow1_albedo = "Data\\Textures\\Floor\\narrow1_albedo.png";
	std::string Narrow1_specular = "";
	std::string Narrow1_normal = "Data\\Textures\\Floor\\narrow1_normal.png";
	std::string Narrow1_roughess = "Data\\Textures\\Floor\\narrow1_roughness.png";
	std::string Narrow1_metallic = "";
	std::string Narrow1_AO = "Data\\Textures\\Floor\\narrow1_ao.png";

	std::string Grass1_albedo = "Data\\Textures\\Floor\\grass1_albedo.png";
	std::string Grass1_specular = "";
	std::string Grass1_normal = "Data\\Textures\\Floor\\grass1_normal.png";
	std::string Grass1_roughess = "Data\\Textures\\Floor\\grass1_rough.png";
	std::string Grass1_metallic = "Data\\Textures\\Floor\\grass1_height.png";
	std::string Grass1_AO = "Data\\Textures\\Floor\\grass1_ao.png";

	std::string Stone1_albedo = "Data\\Textures\\Floor\\stone_albedo.png";
	std::string Stone1_specular = "";
	std::string Stone1_normal = "Data\\Textures\\Floor\\stone_normal.png";
	std::string Stone1_roughess = "Data\\Textures\\Floor\\stone_roughness.png";
	std::string Stone1_metallic = "";
	std::string Stone1_AO = "Data\\Textures\\Floor\\stone_ao.png";
};

