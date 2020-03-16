#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Model
{
public:
	Model();
	Model(glm::mat4 _modelMatrix);
	~Model();

	/* Devuelve la ModelMatrix del objeto*/
	glm::mat4 getModelMatrix();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void Scale(float x, float y, float z);
	void Rotate(float angle, glm::vec3 axesRotation);
	void Translate(float x, float y, float z);

private:
	glm::mat4 modelMatrix;
};
