#pragma once

#include <glm.hpp>

class CPlane
{
public:
	CPlane();
	CPlane(glm::vec3 N, glm::vec3 p);
	CPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	float GetDistance(glm::vec3 p);

	glm::vec3 N;
	float d;
};

