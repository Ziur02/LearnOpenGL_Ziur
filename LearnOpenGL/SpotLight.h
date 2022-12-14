#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class SpotLight
{
public:
	SpotLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 color;
	glm::vec3 direction = glm::vec3(0, 0, 1.0f);

	float cosPhyInner = 0.96f;
	float cosPhyOuter = 0.94f;
	float constant;
	float linear;
	float quadratic;

	void UpdateDirection();
};

