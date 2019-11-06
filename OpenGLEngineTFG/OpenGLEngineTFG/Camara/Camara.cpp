#include "Camara.h"

#include <gtc/matrix_transform.hpp>

Camara::Camara(float fov, int width, int height, float zNear, float zFar): mView(glm::mat4(1.0f)), fov(fov), zNear(zNear), zFar(zFar) {

	//Datos para el ratón
	this->lastX = 400;
	this->lastY = 300;

	this->yaw = -15.0;
	this->pitch = -70.0;

	this->mProjection = glm::perspective( glm::radians(fov), float(width) / float(height), zNear, zFar);

	// Parsear desde Data/Config??
	this->vecPositionCamera = glm::vec3(0.0f, 0.0f, -3.0f);
	this->vecLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	this->vecUp = glm::vec3(0.0f, 1.0f, 0.0f);

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, vecUp);
}

Camara::~Camara() {}

/* Movimiento de la cámara */
void Camara::truck(float value)
{
	this->n = normalize(vecPositionCamera - vecLookAt);
	n *= value;

	this->u = normalize(cross(n, glm::vec3(0, 1, 0)));
	this->v = normalize(cross(u, n));

	vecPositionCamera += n;
	vecLookAt += n;

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, v);
}

void Camara::dolly(float value)
{
	this->n = normalize(vecPositionCamera - vecLookAt);
	this->u = normalize(cross(n, glm::vec3(0, 1, 0)));
	this->v = normalize(cross(u, n));

	u *= value;

	vecPositionCamera += u;
	vecLookAt += u;

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, v);
}

void Camara::boom_crane(float value)
{
	this->n = normalize(vecPositionCamera - vecLookAt);
	this->u = normalize(cross(n, glm::vec3(0, 1, 0)));
	this->v = normalize(cross(u, n));

	v *= value;

	vecPositionCamera += v;
	vecLookAt += v;

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, vecUp);
}


void Camara::moveCamara(float xPosition, float yPosition)
{
	//Actualizamos las posiciones para la proxima iteración
	this->lastX = xPosition;
	this->lastY = yPosition;

	// Obtenemos el yaw y pitch. (Rotación en 'x' e 'y' respectivamente)
	this->yaw += (xPosition - this->lastX) * 0.05f;
	this->pitch += (this->lastY - yPosition) * 0.05f;

	// Para que no se de la vuelta si superas dicho parámetros.
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Spherical coordinates (r=1).
	this->n.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	this->n.y = sin(glm::radians(pitch));
	this->n.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	this->n = normalize(n);
	this->u = normalize(cross(n, glm::vec3(0, 1, 0)));
	this->v = normalize(cross(u, n));
	this->vecLookAt = this->vecPositionCamera + this->n;

	this->mView = glm::lookAt(vecPositionCamera, vecLookAt, v);
}

/* Comprueba si se ha pulsado las teclas para actualizar su estado*/
void Camara::UpdateCamera()
{
	if (InputManager::getInstance()->isAnyButtonPressed()) {

		if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
			this->dolly(0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
			this->dolly(-0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_W)) {
			this->truck(-0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_S)) {
			this->truck(0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_LEFT_CONTROL)) {
			this->boom_crane(0.2f);
		}

		if (InputManager::getInstance()->getInputButtonDown(Key_SPACE)) {
			this->boom_crane(-0.2f);
		}
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
	return this->mProjection;
}

glm::mat4 Camara::getMatrixViewProjection()
{
	return this->mProjection * this->mView;
}
