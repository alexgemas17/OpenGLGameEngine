#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "../Render/SceneObj.h"



class Plane
{
public:
	Plane(float size);
	~Plane();

	SceneObj* getSceneObj();

private:
	//Obj
	SceneObj* obj;

	std::string floor_albedo = "Data\\Textures\\Floor\\floor1_albedo.png";
	std::string floor_specular = "";
	std::string floor_normal = "Data\\Textures\\Floor\\floor1_normal.png";
};

