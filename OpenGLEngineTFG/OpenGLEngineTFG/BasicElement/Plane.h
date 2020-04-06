#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "../Render/SceneObj.h"

enum TypeFloor {
	Grass1, Narrow1
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

	std::string Grass1_albedo = "Data\\Textures\\Floor\\grass1_albedo.png";
	std::string Grass1_specular = "";
	std::string Grass1_normal = "Data\\Textures\\Floor\\grass1_normal.png";
};

