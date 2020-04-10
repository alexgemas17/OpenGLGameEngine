#include "CPlane.h"

const float EPSILON= 0.0001f;

CPlane::CPlane(): N(glm::vec3(0, 1, 0)), d(0.0f)
{}

CPlane::CPlane(glm::vec3 normal, glm::vec3 p) 
{
	N = normal;
	d = -glm::dot(N,p);
}

CPlane::CPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) 
{
	glm::vec3 e1 = v2-v1;
	glm::vec3 e2 = v3-v1;
	N = glm::normalize(glm::cross(e1,e2)); 
	d = -glm::dot(N, v2);
}

float CPlane::GetDistance(glm::vec3 p) {
	return glm::dot(N,p) + d;
}