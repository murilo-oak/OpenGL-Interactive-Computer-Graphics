#pragma once
#include "../glm/glm.hpp"

class Camera
{
public:
	glm::vec3 m_position{};
	float m_radius = 3;

	void updatePosition(float viewAngleX, float viewAngleY, float deltaRadius = 0) {
		m_radius += deltaRadius;

		m_position = glm::vec3(
			m_radius * sin(viewAngleY) * sin(viewAngleX),
			m_radius * cos(viewAngleY),
			m_radius * sin(viewAngleY) * cos(viewAngleX)
		);
	}

	Camera(glm::vec3 position = glm::vec3(), float radius = 3.f) {
		assert(radius != 0);

		m_position = position;
		m_radius = radius;
	}

private:
};

