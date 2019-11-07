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
	void moveCamara(float xPosition, float yPosition, GLboolean constrainPitch = true);

	/* Getters */
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getMatrixViewProjection();

private:
	// Matrices para el render
	glm::mat4 mView;
	glm::mat4 mProjection;

	//Sistema Coordenadas nuv
	glm::vec3 n;				// Vector delante
	glm::vec3 u;				// Vector derecha
	glm::vec3 v;				// Vector arriba

	//Datos de la cámara (Posición, a donde mira y vector de arriba)
	glm::vec3 vecPositionCamera;
	glm::vec3 vecLookAt;
	glm::vec3 vecUp;

	float sensibilidad;
	float velocidadCamara;
	float yaw;
	float pitch;

	float fov;
	float zNear;
	float zFar;

	//Datos para el ratón
	bool primerMovRaton;
	float lastX;
	float lastY;


	/* Funciones privadas */
	void updateCamaraData();
};
