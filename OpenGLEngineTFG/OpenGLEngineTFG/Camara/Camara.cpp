#include "Camara.h"

#include <gtc/matrix_transform.hpp>

Camara::Camara(float fov, int width, int height, float zNear, float zFar): 
	mView(glm::mat4(1.0f)), fov(fov), zNear(zNear), zFar(zFar), primerMovRaton(true),
	u(glm::vec3(0.0f, 0.0f, -1.0f)), sensibilidad(0.1f), velocidadCamara(2.5f),
	yaw(-90.0f), pitch(0.0f)
{

	//Datos para el ratón
	this->lastX = 400;
	this->lastY = 300;

	//Matriz de proyección
	this->mProjection = glm::perspective( glm::radians(fov), float(width) / float(height), zNear, zFar);

	// Parsear desde Data/Config??
	this->vecPositionCamera = glm::vec3(0.0f, 0.0f, 3.0f);
	this->vecLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	this->vecUp = glm::vec3(0.0f, 1.0f, 0.0f);

	updateCamaraData();
}

Camara::~Camara() {}

/* Comprueba si se ha pulsado las teclas para actualizar su estado*/
void Camara::UpdateCamera()
{
	//float velocidadDeltaTime = velocidadCamara * deltaTime; --> Usar esa velocidad en vez de la que tenemos ahora.
	if (InputManager::getInstance()->isAnyButtonPressed()) {

		if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
			this->vecPositionCamera -= this->u * velocidadCamara * 0.02f;
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
			this->vecPositionCamera += this->u * velocidadCamara * 0.02f;
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_W)) {
			this->vecPositionCamera += this->n * velocidadCamara * 0.02f;
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_S)) {
			this->vecPositionCamera -= this->n * velocidadCamara * 0.02f;
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_LEFT_CONTROL)) {
			//this->boom_crane(0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_SPACE)) {
			//this->boom_crane(-0.2f);
		}

		this->mView = glm::lookAt(vecPositionCamera, vecPositionCamera + n, v);
	}
}

void Camara::updateCamaraData()
{
	// Calculamos el vector adelante
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	// Y lo normalizamos 
	this->n = glm::normalize(front);

	// Como hemos actualizado n, actualizamos u y v.
	this->u = glm::normalize(glm::cross(this->n, vecUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->v = glm::normalize(glm::cross(this->u, this->n));

	this->mView = glm::lookAt(vecPositionCamera, vecPositionCamera + n, v);
}

void Camara::moveCamara(float xPosition, float yPosition, GLboolean constrainPitch)
{
	if (primerMovRaton)
	{
		lastX = xPosition;
		lastY = yPosition;
		primerMovRaton = false;
	}

	float xoffset = xPosition - lastX;
	float yoffset = lastY - yPosition;

	lastX = xPosition;
	lastY = yPosition;

	xoffset *= sensibilidad;
	yoffset *= sensibilidad;

	this->yaw += xoffset;
	this->pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->pitch > 89.0f)
			this->pitch = 89.0f;
		if (this->pitch < -89.0f)
			this->pitch = -89.0f;
	}

	//Actualizar valores
	updateCamaraData();
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
	return this->mProjection;
}

glm::mat4 Camara::getMatrixViewProjection()
{
	return this->mProjection * this->mView;
}
