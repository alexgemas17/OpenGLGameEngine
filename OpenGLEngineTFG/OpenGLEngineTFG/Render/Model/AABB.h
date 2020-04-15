#pragma once

#include "glm.hpp"
#include <vector>

class AABB
{
public:
	AABB(glm::vec3 min, glm::vec3 max);
	bool intersectAABB_Plane(glm::vec4 plane, glm::mat4& ModelViewMatrix);

private:
	glm::vec3 min;
	glm::vec3 max;

	std::vector<glm::vec3> corners;

	void CalcCornes();
};

