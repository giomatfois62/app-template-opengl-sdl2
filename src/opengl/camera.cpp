#include "camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera()
{

}

mat4 Camera::view()
{
	if (updated) {
		updateView();
		updated = false;
	}

	return viewMatrix;
}

mat4 Camera::projection()
{
	return projMatrix;
}

vec3 Camera::direction()
{
	glm::vec3 camFront;

	camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	camFront.y = sin(glm::radians(rotation.x));
	camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	camFront = glm::normalize(camFront);

	return camFront;
}

void Camera::setType(CameraType type)
{
	this->type = type;
}

void Camera::setPerspective(float fov, float aspect, float znear, float zfar)
{
	this->fov = fov;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;
	projMatrix = glm::perspective(glm::radians(fov), aspect, znear, zfar);
}

void Camera::setOrtho(float left, float right, float top, float bottom, float znear, float zfar)
{
	this->znear = znear;
	this->zfar = zfar;
	projMatrix = glm::ortho(left, right, top, bottom, znear, zfar);
}

void Camera::updateAspectRatio(float aspect)
{
	this->aspect = aspect;
	projMatrix = glm::perspective(glm::radians(fov), aspect, znear, zfar);
}

void Camera::setPosition(vec3 position)
{
	this->position = position;
	updated = true;
}

void Camera::setRotation(vec3 rotation)
{
	this->rotation = rotation;
	updated = true;
}

void Camera::rotate(vec3 delta)
{
	this->rotation += delta;
	updated = true;
}

void Camera::setTranslation(vec3 translation)
{
	this->position = translation;
	updated = true;
}

void Camera::translate(vec3 delta)
{
	this->position += delta;
	updated = true;
}

void Camera::setFov(float fov)
{
	this->fov = fov;
	projMatrix = glm::perspective(glm::radians(fov), aspect, znear, zfar);
}

void Camera::updateView()

{
	glm::mat4 rotM = glm::mat4(1.0f);
	glm::mat4 transM;

	rotM = glm::rotate(rotM, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 translation = position;
	transM = glm::translate(glm::mat4(1.0f), translation);

	if (type == FPS)
		viewMatrix = rotM * transM;
	else
		viewMatrix = transM * rotM;

	viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
}

void Camera::fitInView(float xMin, float xMax, float yMin, float yMax)
{
	//TODO
}

float Camera::getFov() const
{
	return fov;
}

float Camera::getAspect() const
{
	return aspect;
}

float Camera::getZnear() const
{
	return znear;
}

float Camera::getZfar() const
{
	return zfar;
}


