#include "AABB.h"

#include <gtc/matrix_transform.hpp>

AABB::AABB(glm::vec3 min, glm::vec3 max):
	min(min), max(max)
{
	CalcCornes();
}

void AABB::CalcCornes()
{
	float offset = 0.5;

	corners.push_back(min);
	corners.push_back(max);
	corners.push_back(glm::vec3(min.x, min.y, max.z));
	corners.push_back(glm::vec3(min.x, max.y, min.z));
	corners.push_back(glm::vec3(max.x, min.y, max.z));
	corners.push_back(glm::vec3(min.x, max.y, max.z));
	corners.push_back(glm::vec3(max.x, min.y, max.z));
	corners.push_back(glm::vec3(max.x, max.y, min.z));
}

bool AABB::intersectAABB_Plane(glm::vec4 plane, glm::mat4& ModelViewMatrix)
{
	bool result = false;
	for (unsigned int cornerIndex = 0; cornerIndex< corners.size(); cornerIndex++)
	{
		glm::vec4 corner = ModelViewMatrix * glm::vec4(corners[cornerIndex], 1.0f);
		if (glm::dot(corner, plane) > 0.0f)
		{
			result = true;
			break;
		}
	}

	return result;
}	