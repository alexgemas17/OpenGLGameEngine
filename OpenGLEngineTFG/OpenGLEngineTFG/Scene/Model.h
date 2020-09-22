#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class AABB;

class Model
{
public:
	Model();
	Model(glm::vec3 min, glm::vec3 max);
	Model(glm::mat4 _modelMatrix);

	glm::mat4 getModelMatrix();

	void Scale(float x, float y, float z);
	void Rotate(float angle, glm::vec3 axesRotation);
	void Translate(float x, float y, float z);

private:
	glm::mat4 modelMatrix;
};
