#pragma once

#include <glm.hpp>

#include "../Input/InputManager.h"

class Camara
{
public:
	Camara(float fov, int width, int height, float zNear, float zFar);
	~Camara();

	/* Funciones públicas */
	void UpdateCamera(); // Comprueba si se ha pulsado las teclas para actualizar su estado
	void SetProjection(float newWidth, float newHeight);
	void moveCamara(float xPosition, float yPosition);

	/* Getters */
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getVP();

private:
	// Matrices para el render
	glm::mat4 mView;
	glm::mat4 mProjection;

	//Datos de la cámara (Posición, a donde mira y vector de arriba)
	glm::vec3 vecPositionCamera;
	glm::vec3 vecLookAt;
	glm::vec3 vecUp;

	float fov;
	float zNear;
	float zFar;

	//Datos para el ratón
	bool primerMovRaton;
	float lastX;
	float lastY;

	float yaw;
	float pitch;
};
