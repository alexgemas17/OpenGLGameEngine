#include "Camara.h"

#include "../Render/Model/AABB.h"

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

	CalcFrustumPlanes(mVP);
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
/* Copyright (c) 2015-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

glm::vec3 vec_project(const glm::vec4& vec)
{
	return glm::vec3(vec) / glm::vec3(vec.w);
}

void Camara::CalcFrustumPlanes(glm::mat4 mViewProj) 
{
	// Frustum planes are in world space.
	glm::mat4 inv = glm::inverse(mViewProj);

	// Get world-space coordinates for clip-space bounds.
	glm::vec4 lbn = inv * glm::vec4(-1, -1, -1, 1);
	glm::vec4 ltn = inv * glm::vec4(-1, 1, -1, 1);
	glm::vec4 lbf = inv * glm::vec4(-1, -1, 1, 1);
	glm::vec4 rbn = inv * glm::vec4(1, -1, -1, 1);
	glm::vec4 rtn = inv * glm::vec4(1, 1, -1, 1);
	glm::vec4 rbf = inv * glm::vec4(1, -1, 1, 1);
	glm::vec4 rtf = inv * glm::vec4(1, 1, 1, 1);

	// Divide by w.
	glm::vec3 lbn_pos = vec_project(lbn);
	glm::vec3 ltn_pos = vec_project(ltn);
	glm::vec3 lbf_pos = vec_project(lbf);
	glm::vec3 rbn_pos = vec_project(rbn);
	glm::vec3 rtn_pos = vec_project(rtn);
	glm::vec3 rbf_pos = vec_project(rbf);
	glm::vec3 rtf_pos = vec_project(rtf);

	// Get plane normals for all sides of frustum.
	glm::vec3 left_normal =		glm::normalize(glm::cross(lbf_pos - lbn_pos, ltn_pos - lbn_pos));
	glm::vec3 right_normal =	glm::normalize(glm::cross(rtn_pos - rbn_pos, rbf_pos - rbn_pos));
	glm::vec3 top_normal =		glm::normalize(glm::cross(ltn_pos - rtn_pos, rtf_pos - rtn_pos));
	glm::vec3 bottom_normal =	glm::normalize(glm::cross(rbf_pos - rbn_pos, lbn_pos - rbn_pos));
	glm::vec3 near_normal =		glm::normalize(glm::cross(ltn_pos - lbn_pos, rbn_pos - lbn_pos));
	glm::vec3 far_normal =		glm::normalize(glm::cross(rtf_pos - rbf_pos, lbf_pos - rbf_pos));

	// Plane equations compactly represent a plane in 3D space.
	// We want a way to compute the distance to the plane while preserving the sign to know which side we're on.
	// Let:
	//    O: an arbitrary point on the plane
	//    N: the normal vector for the plane, pointing in the direction
	//       we want to be "positive".
	//    X: Position we want to check.
	//
	// Distance D to the plane can now be expressed as a simple operation:
	// D = dot((X - O), N) = dot(X, N) - dot(O, N)
	//
	// We can reduce this to one dot product by assuming that X is four-dimensional (4th component = 1.0).
	// The normal can be extended to four dimensions as well:
	// X' = vec4(X, 1.0)
	// N' = vec4(N, -dot(O, N))
	//
	// The expression now reduces to: D = dot(X', N')
	frustumPlanes[0] = glm::vec4(near_normal,	-glm::dot(near_normal, lbn_pos));   // Near
	frustumPlanes[1] = glm::vec4(far_normal,	-glm::dot(far_normal, lbf_pos));    // Far
	frustumPlanes[2] = glm::vec4(left_normal,	-glm::dot(left_normal, lbn_pos));   // Left
	frustumPlanes[3] = glm::vec4(right_normal,	-glm::dot(right_normal, rbn_pos));  // Right
	frustumPlanes[4] = glm::vec4(top_normal,	-glm::dot(top_normal, ltn_pos));    // Top
	frustumPlanes[5] = glm::vec4(bottom_normal, -glm::dot(bottom_normal, lbn_pos)); // Bottom
}

bool Camara::isPointInFrustum(AABB& aabb, glm::mat4& ModelViewMatrix)
{
	for (unsigned int p = 0; p < 6; p++)
	{
		bool inside_plane = aabb.intersectAABB_Plane(frustumPlanes[p], ModelViewMatrix);
		
		if (!inside_plane)
			return false;
	}

	return true;
}