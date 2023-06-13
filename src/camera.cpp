#include "camera.h"

Camera::Camera(glm::vec3 position, float radius) 
{
	assert(radius != 0);

	m_position = position;
	m_radius = radius;
}

Camera::~Camera() {}

void Camera::update() 
{
	updatePosition();
	setViewMat();
	updateMVP();
}

void Camera::updatePosition(float deltaRadius) 
{
	m_radius += deltaRadius;

	m_position = glm::vec3(
		m_radius * sin(m_angle.y) * sin(m_angle.x),
		m_radius * cos(m_angle.y),
		m_radius * sin(m_angle.y) * cos(m_angle.x)
	);
}

void Camera::setMVP(unsigned int windowHeight, unsigned int windowWidth) 
{
	setProjectionMat(windowHeight, windowWidth);
	setViewMat();
	setModelMat();

	updateMVP();
}

void Camera::setProjectionMat(unsigned int windowHeight, unsigned int windowWidth) 
{
	m_projection = glm::perspective(glm::radians(55.0f), (float)windowHeight / (float)windowWidth, 0.5f, 100.0f);
}

void Camera::setViewMat() 
{
	m_view = glm::lookAt(
		m_position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
}

void Camera::setModelMat() 
{
	m_model = glm::mat4(1.0f);
};

void Camera::updateMVP() 
{
	m_mvp = m_projection * m_model * m_view;

	m_mv4 = m_model * m_view;
	m_invMv4 = glm::inverse(m_mv4);
	m_mv = glm::transpose(m_invMv4);
}
