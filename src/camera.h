#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"

class Camera
{
public:
	
	glm::vec3 m_position{};
	float m_radius = 3;
	
	glm::mat4 m_projection{};
	glm::mat4 m_model{};
	glm::mat4 m_view{};

	glm::mat3 m_mv{};
	glm::mat4 m_mv4{};
	glm::mat4 m_mvp{};
	glm::mat4 m_invMv4{};

	Camera(glm::vec3 position = glm::vec3(), float radius = 3.f) {
		assert(radius != 0);

		m_position = position;
		m_radius = radius;
	}

	void updatePosition(float viewAngleX, float viewAngleY, float deltaRadius = 0) {
		m_radius += deltaRadius;

		m_position = glm::vec3(
			m_radius * sin(viewAngleY) * sin(viewAngleX),
			m_radius * cos(viewAngleY),
			m_radius * sin(viewAngleY) * cos(viewAngleX)
		);
	}

	void setViewMat() {
		m_view = glm::lookAt(
			m_position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}

	void setProjectionMat(unsigned int windowHeight, unsigned int windowWidth) {
		m_projection = glm::perspective(glm::radians(55.0f), (float)windowHeight / (float)windowWidth, 0.5f, 100.0f);
	}

	void setModelMat() {
		m_model = glm::mat4(1.0f);
	};

	void updateMVP() {
		m_mvp = m_projection * m_model * m_view;

		m_mv4 = m_model * m_view;
		m_invMv4 = glm::inverse(m_mv4);
		m_mv = glm::transpose(m_invMv4);
	}

	void setMVP(unsigned int windowHeight, unsigned int windowWidth) {
		setProjectionMat(windowHeight, windowWidth);
		setViewMat();
		setModelMat();

		updateMVP();
	}

private:
};

