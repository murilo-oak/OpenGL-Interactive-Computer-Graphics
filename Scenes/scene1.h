#pragma once
#include "../src/scene.h"

class Scene1 : public IScene
{
public:
	Camera   m_cam{};
	Object3D m_object3D{};
	Plane    m_plane{};
	Cubemap  m_cubemap{};

	cy::GLSLProgram m_objectProgram{};
	cy::GLSLProgram m_planeProgram{};
	cy::GLSLProgram m_skyboxProgram{};

	glm::vec3 m_lightDir{ 0.0f, 1.0f, 0.0f };
	unsigned int m_windowHeight{}, m_windowWidth{};

	void setup(unsigned int windowWidth, unsigned int windowHeight) override;
	void update() override;
	void render() override;
	void reshapeWindow(unsigned int windowWidth, unsigned int windowHeight) override;

	void onRightButton(MouseInput mouse) override;
	void onLeftButton (MouseInput mouse) override;
	void onLeftButton2(MouseInput mouse) override;

	void setUniformVariables   (GLuint programID, unsigned int windowHeight, unsigned int windowWidth);
	void updateUniformVariables(GLuint programID);
	
	void recompileShaders() override;
};

