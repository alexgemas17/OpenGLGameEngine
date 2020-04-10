#pragma once

/*
	Más info: https://learnopengl.com/Getting-started/Camera
*/

#include <glm.hpp>
#include "../Input/InputManager.h"
#include "../BasicElement/CPlane.h"

class Camara
{
public:
	Camara(float fov, int width, int height, float zNear, float zFar);
	~Camara();

	/* Funciones públicas */
	void UpdateCamera(float deltaTime); // Comprueba si se ha pulsado las teclas para actualizar su estado
	void SetProjection(int newWidth, int newHeight);
	void moveCamara(float xPosition, float yPosition, GLboolean constrainPitch = true);

	/* Getters */
	glm::vec3 getPosition() const;
	glm::vec3 getLookAt() const;
	glm::vec3 getUP() const;
	glm::vec3 getFront() const;
	glm::mat4 getView() const;
	glm::mat4 getProjection() const;
	glm::mat4 getMatrixViewProjection() const;

	glm::mat4 getNewLookAt(glm::vec3 Position, glm::vec3 Direction, glm::vec3 UP);

	void CalcFrustumPlanes();
	bool IsPointInFrustum(glm::vec3 point);
	bool IsSphereInFrustum(glm::vec3 center, float radius);
	bool IsBoxInFrustum(glm::vec3 min, glm::vec3 max);

	//frustum points
	std::vector< glm::vec3> farPts;
	std::vector< glm::vec3> nearPts;

	std::vector<glm::vec4> GetFrustumPlanes();

private:
	// Matrices para el render
	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::mat4 mVP;

	//Sistema Coordenadas nuv
	glm::vec3 n;				// Vector delante
	glm::vec3 u;				// Vector derecha
	glm::vec3 v;				// Vector arriba

	//Frsutum planes
	std::vector< CPlane*> frustumPlanes;

	//Datos de la cámara (Posición, a donde mira y vector de arriba)
	glm::vec3 vecPositionCamera;
	glm::vec3 vecLookAt;
	glm::vec3 vecUp;

	float sensibilidad;
	float velocidadCamara;
	float yaw;
	float pitch;

	float aspectRatio;
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
