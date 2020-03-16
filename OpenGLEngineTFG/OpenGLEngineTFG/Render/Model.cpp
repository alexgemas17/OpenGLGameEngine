#include "Model.h"

Model::Model() : modelMatrix(glm::mat4(1.0f)) {}

Model::Model(glm::mat4 _modelMatrix) : modelMatrix(_modelMatrix) {}

Model::~Model() {}

glm::mat4 Model::getModelMatrix() {
	return this->modelMatrix;
}

void Model::Scale(float x, float y, float z)
{
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(x, y, z));
}

void Model::Rotate(float angle, glm::vec3 axesRotation)
{
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(angle), axesRotation);
}

void Model::Translate(float x, float y, float z)
{
	this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(x, y, z));
}

glm::vec3 Model::getPosition()
{
	return glm::vec3(0, 0, 0);
}

glm::vec3 Model::getRotation()
{
	return glm::vec3(0, 0, 0);
}

glm::vec3 Model::getScale()
{
	return glm::vec3(0, 0, 0);
}