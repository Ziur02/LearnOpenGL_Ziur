#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup)
{
	WorldUp = worldup;
	cameraPos = position;
	cameraFront = glm::normalize(target - position);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldup));
	cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));
}

Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup)
	:cameraFront(0.0f, 0.0f, -1.0f), MovementSpeed(2.5f), Sensitivity(0.1f), Zoom(45.0f)
{
	WorldUp = worldup;
	cameraPos = position;
	Pitch = pitch;
	Yaw = yaw;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, WorldUp);
}

void Camera::UpdateCameraVectors()
{
	cameraFront.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	cameraFront.y = sin(glm::radians(Pitch));
	cameraFront.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
	cameraRight = glm::normalize(glm::cross(cameraFront, WorldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	deltaX *= Sensitivity;
	deltaY *= Sensitivity;

	Yaw += deltaX;
	Pitch += deltaY;

	if (Pitch > 89.0f) Pitch = 89.0f;
	if (Pitch < -89.0f) Pitch = -89.0f;

	UpdateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float cameraSpeed = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		cameraPos += cameraFront * cameraSpeed;
	if (direction == BACKWARD)
		cameraPos -= cameraFront * cameraSpeed;
	if (direction == LEFT)
		cameraPos -= cameraRight * cameraSpeed;
	if (direction == RIGHT)
		cameraPos += cameraRight * cameraSpeed;
	if (direction == UP)
		cameraPos += cameraUp * cameraSpeed;
	if (direction == DOWN)
		cameraPos -= cameraUp * cameraSpeed;
}

void Camera::ProcessMouseScroll(float deltaY)
{
	Zoom -= (float)deltaY;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}