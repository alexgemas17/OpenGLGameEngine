#include "Camara.h"

#include <gtc/matrix_transform.hpp>

Camara::Camara(float fov, int width, int height, float zNear, float zFar): mView(glm::mat4(1.0f)), fov(fov), zNear(zNear), zFar(zFar) {

	//Datos para el ratón
	this->lastX = 400;
	this->lastY = 300;

	this->yaw = -15.0;
	this->pitch = -70.0;

	this->mProjection = glm::perspective( glm::radians(fov), width / float(height), zNear, zFar);

	// Parsear desde Data/Config??
	this->vecPositionCamera = glm::vec3(0.0f, 0.0f, -3.0f);
	this->vecLookAt = glm::vec3(0.0f, 0.0f, 10.0f);
	this->vecUp = glm::vec3(0.0f, 1.0f, 0.0f);

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, vecUp);
}

Camara::~Camara() {}


void Camara::moveCamara(float xPosition, float yPosition)
{
	//Actualizamos las posiciones para la proxima iteración
	this->lastX = xPosition;
	this->lastY = yPosition;

	// Obtenemos el yaw y pitch.
	this->yaw += (xPosition - this->lastX) * 0.05;
	this->pitch += (this->lastY - yPosition) * 0.05;


	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Spherical coordinates (r=1).
	n.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	n.y = sin(glm::radians(pitch));
	n.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	this->n = normalize(n);
	this->u = normalize(cross(n, glm::vec3(0, 1, 0)));
	this->v = normalize(cross(u, n));
	this->vecLookAt = vecPositionCamera + n;

	this->View = glm::lookAt(vecPositionCamera, vecLookAt, v);
}

/* Comprueba si se ha pulsado las teclas para actualizar su estado*/
void Camara::UpdateCamera()
{
	if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
		// TODO
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
		// TODO
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_W)) {
		// TODO
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_S)) {
		// TODO
	}
}

/* Cambia la matriz de proyeccion según el nuevo alto y ancho que tenga la ventana */
void Camara::SetProjection(float newWidth, float newHeight)
{
	this->mProjection = glm::perspective( glm::radians( this->fov), newWidth / float(newHeight), this->zNear, this->zFar );
}

/* Getters */
glm::mat4 Camara::getView()
{
	return this->mView;
}

glm::mat4 Camara::getProjection()
{
	return this->getProjection;
}

glm::mat4 Camara::getVP()
{
	return this->mProjection * this->mView;
}
