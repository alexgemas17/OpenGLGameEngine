#include "Camara.h"

#include <gtc/matrix_transform.hpp>

/*
	Sensibilidad: velocidad de movimiento de la cámara.
	velocidadCamara: velocidad con la que se mueve la cámara normalmente.

	TO-DO: Añadir también para poder ser manipurable la velocidad con shift.
*/
Camara::Camara(float fov, int width, int height, float zNear, float zFar): 
	mView(glm::mat4(1.0f)), fov(fov), zNear(zNear), zFar(zFar), primerMovRaton(true),
	u(glm::vec3(0.0f, 0.0f, -1.0f)), sensibilidad(0.2f), velocidadCamara(15.0f),
	yaw(-90.0f), pitch(0.0f), aspectRatio(float(width) / float(height))
{

	//Datos para el ratón
	this->lastX = 400;
	this->lastY = 300;

	//Matriz de proyección
	this->mProjection = glm::perspective( glm::radians(fov), aspectRatio, zNear, zFar);

	// Parsear desde Data/Config??
	this->vecPositionCamera = glm::vec3(0.0f, 0.0f, 2.0f);
	this->vecLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	this->vecUp = glm::vec3(0.0f, 1.0f, 0.0f);

	updateCamaraData();
}

Camara::~Camara() {}

/* Comprueba si se ha pulsado las teclas para actualizar su estado*/
void Camara::UpdateCamera(float deltaTime)
{
	bool teclaPulsada = false;
	float velocidadDeltaTime = 0.0f;

	//Para ir más rápido
	if (InputManager::getInstance()->getInputButtonDown(Key_LEFT_SHIFT)) {
		velocidadDeltaTime = (velocidadCamara * 10.0f) * deltaTime;
	}
	else {
		velocidadDeltaTime = velocidadCamara * deltaTime;
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
		teclaPulsada = true;
		this->vecPositionCamera -= this->u * velocidadDeltaTime;
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
		teclaPulsada = true;
		this->vecPositionCamera += this->u * velocidadDeltaTime;
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_W)) {
		teclaPulsada = true;
		this->vecPositionCamera += this->n * velocidadDeltaTime;
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_S)) {
		teclaPulsada = true;
		this->vecPositionCamera -= this->n * velocidadDeltaTime;
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_LEFT_CONTROL)) {
		teclaPulsada = true;
		//this->boom_crane(0.2f);
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_SPACE)) {
		teclaPulsada = true;
		//this->boom_crane(-0.2f);
	}

	if (teclaPulsada) {
		this->mView = glm::lookAt(vecPositionCamera, vecPositionCamera + n, v);
		this->mVP = this->mProjection * this->mView;
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
	this->mVP = this->mProjection * this->mView;

	CalcFrustumPlanes();
}

glm::mat4 Camara::getNewLookAt(glm::vec3 Position, glm::vec3 Direction, glm::vec3 UP) {
	return glm::lookAt(Position, Direction, UP);
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
void Camara::SetProjection(int newWidth, int newHeight)
{
	this->mProjection = glm::perspective( glm::radians( this->fov), newWidth / float(newHeight), this->zNear, this->zFar );
}

/* Getters */
glm::vec3 Camara::getPosition() const
{
	return this->vecPositionCamera;
}

glm::vec3 Camara::getLookAt() const
{
	return this->vecLookAt;
}

glm::vec3 Camara::getUP() const
{
	return this->vecUp;
}

glm::vec3 Camara::getFront() const
{
	return this->n;
}

glm::mat4 Camara::getView() const
{
	return this->mView;
}

glm::mat4 Camara::getProjection() const
{
	return this->mProjection;
}

glm::mat4 Camara::getMatrixViewProjection() const 
{
	return this->mVP;
}

/* ------------------------- FUNCIONES RELACIONADAS CON EL FRUSTUM CULLING ------------------------ */
void Camara::CalcFrustumPlanes() 
{
	frustumPlanes.clear();
	farPts.clear();
	nearPts.clear();

	glm::vec3 cN = vecPositionCamera + vecLookAt * zNear;
	glm::vec3 cF = vecPositionCamera + vecLookAt * zFar;

	float Hnear = 2.0f * tan(fov / 2.0f) * zNear;
	float Wnear = Hnear * aspectRatio;
	float Hfar = 2.0f * tan(fov / 2.0f) * zFar;
	float Wfar = Hfar * aspectRatio;
	float hHnear = Hnear / 2.0f;
	float hWnear = Wnear / 2.0f;
	float hHfar = Hfar / 2.0f;
	float hWfar = Wfar / 2.0f;


	farPts.push_back(cF + v * hHfar - u * hWfar);
	farPts.push_back(cF - v * hHfar - u * hWfar);
	farPts.push_back(cF - v * hHfar + u * hWfar);
	farPts.push_back(cF + v * hHfar + u * hWfar);

	nearPts.push_back(cN + v * hHnear - u * hWnear);
	nearPts.push_back(cN - v * hHnear - u * hWnear);
	nearPts.push_back(cN - v * hHnear + u * hWnear);
	nearPts.push_back(cN + v * hHnear + u * hWnear);

	frustumPlanes.push_back(new CPlane(nearPts[3], nearPts[0], farPts[0]));
	frustumPlanes.push_back(new CPlane(nearPts[1], nearPts[2], farPts[2]));
	frustumPlanes.push_back(new CPlane(nearPts[0], nearPts[1], farPts[1]));
	frustumPlanes.push_back(new CPlane(nearPts[2], nearPts[3], farPts[2]));
	frustumPlanes.push_back(new CPlane(nearPts[0], nearPts[3], nearPts[2]));
	frustumPlanes.push_back(new CPlane(farPts[3], farPts[0], farPts[1]));
}

bool Camara::IsPointInFrustum(glm::vec3 point) 
{
	for (int i = 0; i < 6; i++)
	{
		if (frustumPlanes[i]->GetDistance(point) < 0)
			return false;
	}
	return true;
}

bool Camara::IsSphereInFrustum(glm::vec3 center, float radius) 
{
	for (int i = 0; i < 6; i++)
	{
		float d = frustumPlanes[i]->GetDistance(center);
		if (d < -radius)
			return false;
	}
	return true;
}


bool Camara::IsBoxInFrustum(glm::vec3 min, glm::vec3 max) 
{
	for (int i = 0; i < 6; i++)
	{
		glm::vec3 p = min, n = max;
		glm::vec3 N = frustumPlanes[i]->N;
		if (N.x >= 0) {
			p.x = max.x;
			n.x = min.x;
		}
		if (N.y >= 0) {
			p.y = max.y;
			n.y = min.y;
		}
		if (N.z >= 0) {
			p.z = max.z;
			n.z = min.z;
		}

		if (frustumPlanes[i]->GetDistance(p) < 0) {
			return false;
		}
	}
	return true;
}

std::vector<glm::vec4> Camara::GetFrustumPlanes()
{
	std::vector<glm::vec4> result;
	for (int i = 0; i < 6; i++)
		result.push_back(glm::vec4(frustumPlanes[i]->N, frustumPlanes[i]->d));

	return result;
}