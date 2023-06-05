#pragma once
#include "../ThirdParty/include/glm/glm.hpp"
#include "../ThirdParty/include/glm/gtc/matrix_transform.hpp"
#include "../ThirdParty/include/glm/gtc/type_ptr.hpp"
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"

class Camera
{
public:
	//position coordinates (x,y,z)
	glm::vec3 m_position{};

	//polar coordinates (angle1, angle2, r)
	glm::vec2 m_angle{ 0.90f, 0.90f };
	float m_radius = 3;
	
	glm::mat4 m_projection{};
	glm::mat4 m_model{};
	glm::mat4 m_view{};

	glm::mat3 m_mv{};
	glm::mat4 m_mv4{};
	glm::mat4 m_mvp{};
	glm::mat4 m_invMv4{};

	Camera(glm::vec3 position = glm::vec3(), float radius = 3.f);
	~Camera();

	void updatePosition(float deltaRadius = 0);
	void setViewMat();
	void setProjectionMat(unsigned int windowHeight, unsigned int windowWidth);
	void setModelMat();
	void updateMVP();
	void setMVP(unsigned int windowHeight, unsigned int windowWidth);
	void update();
};

