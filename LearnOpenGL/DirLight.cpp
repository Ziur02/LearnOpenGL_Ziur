#include "DirLight.h"

DirLight::DirLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color)
	:position(_position),
	angles(_angles),
	color(_color)
{
	UpdateDiretion();
}

void DirLight::UpdateDiretion()
{
	direction = glm::vec3(0, 0, 1.0f);
	direction = glm::rotateZ(direction, angles.z);
	direction = glm::rotateX(direction, angles.x);
	direction = glm::rotateY(direction, angles.y);
	direction *= -1.0f;
}
