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

	/* Movimiento de la cámara */
	void truck(float value);					//Hacia delante/atrás
	void dolly(float value);					//Hacia delante/atrás
	void boom_crane(float value);				//Hacia arriba/abajo

	/* Getters */
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getMatrixViewProjection();

private:
	// Matrices para el render
	glm::mat4 mView;
	glm::mat4 mProjection;

	//Sistema Coordenadas nuv
	glm::vec3 n;
	glm::vec3 u;
	glm::vec3 v;

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
